class SwitchDebug {
    public static void main() {
        int x;
        
        System.out.println("Start");
        x = 1;
        System.out.println("x = 1");
        
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        System.out.println("End");
        return;
    }
}

// Made with Bob
