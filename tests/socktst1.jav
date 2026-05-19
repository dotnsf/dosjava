// socktest.jav - Minimal socket test
class socktest {
    public static void main() {
        int sock;
        System.out.println("Socket Test");
        Socket.init();
        sock = Socket.create("127.0.0.1", 80);
        System.out.println("Done");
        return;
    }
}

// Made with Bob
