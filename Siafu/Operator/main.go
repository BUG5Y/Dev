
package main

import (
    "bufio"
    "fmt"
    "net/http"
    "net/url"
    "os"
    "strings"
	"encoding/json"
)

const (
    serverURL = "http://192.168.0.54:8443/operator" // Replace this with your server URL
)

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
    // Create HTTP client
    client := &http.Client{}

    // Create command URL
    commandURL := serverURL

    // Prepare form data
    formData := url.Values{}
    formData.Set("cmdGroup", cmdGroup)
    formData.Set("cmdString", cmdString)

    // Create HTTP request
    req, err := http.NewRequest("POST", commandURL, strings.NewReader(formData.Encode()))
    if err != nil {
        return err
    }

    // Set headers
    req.Header.Set("Content-Type", "application/x-www-form-urlencoded")

    // Send request
    resp, err := client.Do(req)
    if err != nil {
        return err
    }
    

    // Check the response status code
    if resp.StatusCode != http.StatusOK {
        return fmt.Errorf("unexpected status code: %d", resp.StatusCode)
    }

    // Read and parse the response JSON
    var response struct {
        CmdGroup   string `json:"cmdGroup"`
        CmdString  string `json:"cmdString"`
        CmdResponse string `json:"cmdResponse"`
    }

    err = json.NewDecoder(resp.Body).Decode(&response)
    if err != nil {
        return err
    }

    // Replace for readability
    response.CmdResponse = strings.Replace(response.CmdResponse, "\\n", "\n", -1)
    response.CmdResponse = strings.Replace(response.CmdResponse, "\\r", "\r", -1)

    fmt.Println("CmdResponse:", response.CmdResponse)

	defer resp.Body.Close()
	

    return nil
}