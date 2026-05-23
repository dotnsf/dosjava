// Comprehensive switch statement test
class SwitchTest {
    public static void main() {
        int x;
        long y;
        
        System.out.println("=== Switch Statement Tests ===");
        
        // Test 1: INT switch with multiple cases
        System.out.println("\n1. INT switch:");
        x = 2;
        switch (x) {
            case 1:
                System.out.println("  One");
                break;
            case 2:
                System.out.println("  Two");
                break;
            case 3:
                System.out.println("  Three");
                break;
            default:
                System.out.println("  Other");
                break;
        }
        
        // Test 2: LONG switch
        System.out.println("\n2. LONG switch:");
        y = 200L;
        switch (y) {
            case 100L:
                System.out.println("  One hundred");
                break;
            case 200L:
                System.out.println("  Two hundred");
                break;
            case 300L:
                System.out.println("  Three hundred");
                break;
            default:
                System.out.println("  Other");
                break;
        }
        
        // Test 3: Default case (with default)
        System.out.println("\n3. Default case:");
        x = 99;
        switch (x) {
            case 1:
                System.out.println("  One");
                break;
            case 2:
                System.out.println("  Two");
                break;
            default:
                System.out.println("  Default matched");
                break;
        }
        
        // Test 4: No default case (no match)
        System.out.println("\n4. No default (no match):");
        x = 99;
        switch (x) {
            case 1:
                System.out.println("  One");
                break;
            case 2:
                System.out.println("  Two");
                break;
        }
        System.out.println("  After switch");
        
        // Test 5: Default at beginning
        System.out.println("\n5. Default at beginning:");
        x = 99;
        switch (x) {
            default:
                System.out.println("  Default first");
                break;
            case 1:
                System.out.println("  One");
                break;
            case 2:
                System.out.println("  Two");
                break;
        }
        
        // Test 6: Nested switch
        System.out.println("\n6. Nested switch:");
        x = 1;
        y = 2L;
        switch (x) {
            case 1:
                switch (y) {
                    case 1L:
                        System.out.println("  x=1, y=1");
                        break;
                    case 2L:
                        System.out.println("  x=1, y=2");
                        break;
                    default:
                        System.out.println("  x=1, y=other");
                        break;
                }
                break;
            case 2:
                System.out.println("  x=2");
                break;
        }
        
        // Test 7: Many cases (6 cases)
        System.out.println("\n7. Many cases:");
        x = 5;
        switch (x) {
            case 1: System.out.println("  One"); break;
            case 2: System.out.println("  Two"); break;
            case 3: System.out.println("  Three"); break;
            case 4: System.out.println("  Four"); break;
            case 5: System.out.println("  Five"); break;
            case 6: System.out.println("  Six"); break;
            default: System.out.println("  Other"); break;
        }
        
        System.out.println("\n=== All tests complete ===");
        return;
    }
}