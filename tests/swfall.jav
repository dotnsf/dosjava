// Test: Fall-through behavior
class SwitchFallthrough {
    public static void main() {
        int x;
        
        System.out.println("Testing fall-through behavior");
        
        // Test fall-through: case 1 and 2 both print "One or Two"
        x = 1;
        System.out.print("x=1 (fall-through): ");
        switch (x) {
            case 1:
            case 2:
                System.out.println("One or Two");
                break;
            case 3:
                System.out.println("Three");
                break;
        }
        
        x = 2;
        System.out.print("x=2 (fall-through): ");
        switch (x) {
            case 1:
            case 2:
                System.out.println("One or Two");
                break;
            case 3:
                System.out.println("Three");
                break;
        }
        
        // Test fall-through with statements
        x = 3;
        System.out.print("x=3 (fall-through with code): ");
        switch (x) {
            case 3:
                System.out.print("Three ");
            case 4:
                System.out.println("or Four");
                break;
            case 5:
                System.out.println("Five");
                break;
        }
        
        x = 4;
        System.out.print("x=4 (fall-through with code): ");
        switch (x) {
            case 3:
                System.out.print("Three ");
            case 4:
                System.out.println("or Four");
                break;
            case 5:
                System.out.println("Five");
                break;
        }
        
        // Test multiple fall-through
        x = 1;
        System.out.print("x=1 (multiple fall-through): ");
        switch (x) {
            case 1:
            case 2:
            case 3:
                System.out.println("One, Two, or Three");
                break;
            case 4:
                System.out.println("Four");
                break;
        }
        
        x = 2;
        System.out.print("x=2 (multiple fall-through): ");
        switch (x) {
            case 1:
            case 2:
            case 3:
                System.out.println("One, Two, or Three");
                break;
            case 4:
                System.out.println("Four");
                break;
        }
        
        x = 3;
        System.out.print("x=3 (multiple fall-through): ");
        switch (x) {
            case 1:
            case 2:
            case 3:
                System.out.println("One, Two, or Three");
                break;
            case 4:
                System.out.println("Four");
                break;
        }
        
        System.out.println("Fall-through test complete");
        return;
    }
}