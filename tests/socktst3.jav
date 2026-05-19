// socktst3.jav - Test with two variables
class socktst3 {
    public static void main() {
        int s;
        int c;
        System.out.println("Test");
        Socket.init();
        s = Socket.create("192.168.0.2", 8080);
        c = Socket.isConnected(s);
        System.out.println("Done");
        return;
    }
}

// Made with Bob
