// sockconn.jav - Socket connection test
// Tests Socket.create() native method with connection
class sockconn {
    public static void main() {
        int sock;
        int connected;
        
        System.out.println("Socket Connection Test");
        Socket.init();
        System.out.println("Creating socket...");
        sock = Socket.create("192.168.0.2", 8080);
        System.out.println("Socket created");
        connected = Socket.isConnected(sock);
        if (connected != 0) {
            System.out.println("Socket connected");
        } else {
            System.out.println("Socket not connected");
        }
        Socket.close(sock);
        System.out.println("Socket closed");
        return;
    }
}

// Made with Bob
