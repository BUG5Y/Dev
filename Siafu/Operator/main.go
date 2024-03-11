
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

)

const (
    serverURL = "http://192.168.0.54:8443/operator" // Replace this with your server URL
)

type Command struct {
    Group    string `json:"Group"`
    String  string `json:"String"`
    Response string `json:"Response"`
}

func main() {
    for {
        reader := bufio.NewReader(os.Stdin)
        fmt.Print("> ")
        input, err := reader.ReadString('\n')
        if err != nil {
            fmt.Println("Error reading input:", err)
            continue
        }

        // Trim spaces and newline characters from the input
        input = strings.TrimSpace(input)

        // Parse the input command
        parts := strings.Fields(input)
        if len(parts) < 2 {
            fmt.Println("Invalid command. Please provide a command group and a command.")
            continue
        }

        cmdGroup := parts[0]
        cmdString := strings.Join(parts[1:], " ")

        // Send the command to the server
        err = sendCommand(cmdGroup, cmdString)
        if err != nil {
            fmt.Println("Error sending command:", err)
            continue
        }
    }
}

func sendCommand(cmdGroup, cmdString string) error {
    client := &http.Client{}

    commandURL := serverURL

    cmdData := Command{
        Group:   cmdGroup,
        String:  cmdString,
        Response: "", 
    }

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

    // Send request
    resp, err := client.Do(req)
    if err != nil {
        return err
    }

    defer resp.Body.Close()

    if resp.StatusCode != http.StatusOK {
        return fmt.Errorf("unexpected status code: %d", resp.StatusCode)
    }

    return nil

    // Read the response
    responseBody, err := ioutil.ReadAll(resp.Body)
    if err != nil {
        return fmt.Errorf("error reading response body: %w", err)
    }

    // Base64 decode the data
    decodedData, err := base64.StdEncoding.DecodeString(string(responseBody))
    if err != nil {
        return fmt.Errorf("error decoding base64 data: %w", err)
    }

    // Unmarshal JSON
    var receivedStruct Command
    if err := json.Unmarshal(decodedData, &receivedStruct); err != nil {
        return fmt.Errorf("error unmarshaling JSON data: %w", err)
    }

    // Print or process the command response
    fmt.Println("CmdResponse:", receivedStruct.Response)

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
