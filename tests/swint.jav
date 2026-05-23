// Test: Int switch with multiple cases
class SwitchInt {
    public static void main() {
        int x;
        
        System.out.println("Testing int switch with multiple cases");
        
        // Test case 1
        x = 1;
        System.out.print("x=1: ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
            case 3:
                System.out.println("Three");
                break;
            case 4:
                System.out.println("Four");
                break;
            case 5:
                System.out.println("Five");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        // Test case 3
        x = 3;
        System.out.print("x=3: ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
            case 3:
                System.out.println("Three");
                break;
            case 4:
                System.out.println("Four");
                break;
            case 5:
                System.out.println("Five");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        // Test case 5
        x = 5;
        System.out.print("x=5: ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
            case 3:
                System.out.println("Three");
                break;
            case 4:
                System.out.println("Four");
                break;
            case 5:
                System.out.println("Five");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        // Test default case
        x = 99;
        System.out.print("x=99: ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
            case 3:
                System.out.println("Three");
                break;
            case 4:
                System.out.println("Four");
                break;
            case 5:
                System.out.println("Five");
                break;
            default:
                System.out.println("Other");
                break;
        }
        
        System.out.println("Int switch test complete");
        return;
    }
}