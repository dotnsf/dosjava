// socksend.jav - Socket send/recv test
// Tests Socket.send() and Socket.recv() native methods
class socksend {
    public static void main() {
        int sock;
        int connected;
        String msg;
        int sent;
        String response;
        
        System.out.println("Socket Send/Recv Test");
        Socket.init();
        System.out.println("Creating socket...");
        sock = Socket.create("192.168.0.2", 8080);
        System.out.println("Socket created");
        connected = Socket.isConnected(sock);
        if (connected != 0) {
            System.out.println("Socket connected");
            msg = "GET / HTTP/1.0\r\n\r\n";
            sent = Socket.send(sock, msg);
            System.out.println("Sent bytes:");
            System.out.println(sent);
            response = Socket.recv(sock, 256);
            System.out.println("Received:");
            System.out.println(response);
        } else {
            System.out.println("Socket not connected");
        }
        Socket.close(sock);
        System.out.println("Socket closed");
        return;
    }
}

// Made with Bob
