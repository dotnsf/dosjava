// Test: Long switch
class SwitchLong {
    public static void main() {
        long x;
        
        System.out.println("Testing long switch");
        
        // Test case 100L
        x = 100L;
        System.out.print("x=100L: ");
        switch (x) {
            case 100L:
                System.out.println("One hundred");
                break;
            case 200L:
                System.out.println("Two hundred");
                break;
            case 300L:
                System.out.println("Three hundred");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        // Test case 200L
        x = 200L;
        System.out.print("x=200L: ");
        switch (x) {
            case 100L:
                System.out.println("One hundred");
                break;
            case 200L:
                System.out.println("Two hundred");
                break;
            case 300L:
                System.out.println("Three hundred");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        // Test case 300L
        x = 300L;
        System.out.print("x=300L: ");
        switch (x) {
            case 100L:
                System.out.println("One hundred");
                break;
            case 200L:
                System.out.println("Two hundred");
                break;
            case 300L:
                System.out.println("Three hundred");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        // Test default case
        x = 999L;
        System.out.print("x=999L: ");
        switch (x) {
            case 100L:
                System.out.println("One hundred");
                break;
            case 200L:
                System.out.println("Two hundred");
                break;
            case 300L:
                System.out.println("Three hundred");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        System.out.println("Long switch test complete");
        return;
    }
}