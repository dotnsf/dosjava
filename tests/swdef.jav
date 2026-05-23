// Test: Default case handling
class SwitchDefault {
    public static void main() {
        int x;
        
        System.out.println("Testing default case handling");
        
        // Test with default case
        x = 10;
        System.out.print("x=10 (with default): ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
            default:
                System.out.println("Default case");
                break;
        }
        
        // Test without default case (no match)
        x = 10;
        System.out.print("x=10 (no default): ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
        }
        System.out.println("After switch");
        
        // Test default at beginning
        x = 99;
        System.out.print("x=99 (default first): ");
        switch (x) {
            default:
                System.out.println("Default first");
                break;
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
        }
        
        // Test default in middle
        x = 99;
        System.out.print("x=99 (default middle): ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            default:
                System.out.println("Default middle");
                break;
            case 2:
                System.out.println("Two");
                break;
        }
        
        System.out.println("Default case test complete");
        return;
    }
}