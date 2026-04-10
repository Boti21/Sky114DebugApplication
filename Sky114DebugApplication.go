package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"os/exec"
	"runtime"
	"time"
	"io/fs"
	"embed"
)

// openBrowser opens the specified URL in the default browser of the user.
func openBrowser(url string) {
	var err error

	switch runtime.GOOS {
	case "linux":
		err = exec.Command("xdg-open", url).Start()
	case "windows":
		err = exec.Command("rundll32", "url.dll,FileProtocolHandler", url).Start()
	case "darwin":
		err = exec.Command("open", url).Start()
	default:
		err = fmt.Errorf("unsupported platform")
	}

	if err != nil {
		log.Printf("Could not open browser automatically: %v", err)
	}
}

var folderToServe embed.FS

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: program <ssh-user@ip>")
		fmt.Println("Example: program local@10.197.216.186")
		os.Exit(1)
	}


	sshTarget := os.Args[1]

	// 1. Launch the SSH tunnel in a goroutine
	go func() {
		log.Printf(
			"Starting SSH tunnel: 9001:localhost:9001 via %s",
			sshTarget,
		)

		// Command: ssh -N -L 9001:localhost:9001 local@10.197.216.186
		cmd := exec.Command("ssh", "-N", "-L", "9001:localhost:9001", sshTarget)
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr

		if err := cmd.Run(); err != nil {
			log.Fatalf("SSH tunnel exited with error: %v", err)
		}
	}()

	// 2. Set up the HTTP Server (Equivalent to your Python script)
	// Create a file server that serves the current directory (".")
	serverRoot, _ := fs.Sub(folderToServe, "build/WebAssembly_Qt_6_10_2_multi_threaded-Debug")
	// fileServer := http.FileServer(http.Dir("."))
	fileServer := http.FileServer(http.FS(serverRoot))

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		// Set the exact headers from the Python script
		w.Header().Set("Cross-Origin-Opener-Policy", "same-origin")
		w.Header().Set("Cross-Origin-Embedder-Policy", "require-corp")
		
		// Serve the file
		fileServer.ServeHTTP(w, r)
	})

	// 3. Open the browser automatically to port 8000
	go func() {
		time.Sleep(500 * time.Millisecond)
		openBrowser("http://localhost:8000")
	}()

	log.Println("Serving files from current directory on http://localhost:8000...")

	// Listen on localhost:8000 just like the Python script
	err := http.ListenAndServe("localhost:8000", nil)
	if err != nil {
		log.Fatalf("Failed to start HTTP server: %v", err)
	}
}
