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

func httpRequestHandler(w http.ResponseWriter, req *http.Request) {
	w.Write([]byte("Hello,World!\n"))
}
func main() {
	server := http.Server{
		Addr:      ":8443",
		Handler:   http.HandlerFunc(httpRequestHandler),
	}

	serverIP, err = getServerIP()
    if err != nil {
        fmt.Println("Error:", err)
        return
    }

	if serverIP == "" {
        log.Fatal("Failed to determine server IP address")
    }

	fmt.Printf("Server started at http://%s:8443\n", serverIP)
    err = server.ListenAndServe()
    if err != nil {
        log.Fatal("Failed to start HTTP server: ", err)
    }
	
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