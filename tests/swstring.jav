class StringSwitch {
    public static void main() {
        String s = "hello";
        System.out.println("Testing string switch with: " + s);
        
        switch (s) {
            case "hello":
                System.out.println("Matched: hello");
                break;
            case "world":
                System.out.println("Matched: world");
                break;
            case "test":
                System.out.println("Matched: test");
                break;
            default:
                System.out.println("No match");
                break;
        }
        
        System.out.println("Done");
    }
}

// Made with Bob
