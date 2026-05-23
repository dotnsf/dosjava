// Test: Nested switch statements
class SwitchNested {
    public static void main() {
        int x;
        int y;
        
        System.out.println("Testing nested switch statements");
        
        // Test nested switch
        x = 1;
        y = 2;
        System.out.print("x=1, y=2: ");
        switch (x) {
            case 1:
                switch (y) {
                    case 1:
                        System.out.println("x=1, y=1");
                        break;
                    case 2:
                        System.out.println("x=1, y=2");
                        break;
                    default:
                        System.out.println("x=1, y=other");
                        break;
                }
                break;
            case 2:
                switch (y) {
                    case 1:
                        System.out.println("x=2, y=1");
                        break;
                    case 2:
                        System.out.println("x=2, y=2");
                        break;
                    default:
                        System.out.println("x=2, y=other");
                        break;
                }
                break;
            default:
                System.out.println("x=other");
                break;
        }
        
        // Test another combination
        x = 2;
        y = 1;
        System.out.print("x=2, y=1: ");
        switch (x) {
            case 1:
                switch (y) {
                    case 1:
                        System.out.println("x=1, y=1");
                        break;
                    case 2:
                        System.out.println("x=1, y=2");
                        break;
                    default:
                        System.out.println("x=1, y=other");
                        break;
                }
                break;
            case 2:
                switch (y) {
                    case 1:
                        System.out.println("x=2, y=1");
                        break;
                    case 2:
                        System.out.println("x=2, y=2");
                        break;
                    default:
                        System.out.println("x=2, y=other");
                        break;
                }
                break;
            default:
                System.out.println("x=other");
                break;
        }
        
        // Test default in nested switch
        x = 1;
        y = 99;
        System.out.print("x=1, y=99: ");
        switch (x) {
            case 1:
                switch (y) {
                    case 1:
                        System.out.println("x=1, y=1");
                        break;
                    case 2:
                        System.out.println("x=1, y=2");
                        break;
                    default:
                        System.out.println("x=1, y=other");
                        break;
                }
                break;
            case 2:
                switch (y) {
                    case 1:
                        System.out.println("x=2, y=1");
                        break;
                    case 2:
                        System.out.println("x=2, y=2");
                        break;
                    default:
                        System.out.println("x=2, y=other");
                        break;
                }
                break;
            default:
                System.out.println("x=other");
                break;
        }
        
        System.out.println("Nested switch test complete");
        return;
    }
}