class SwitchTest2 {
    public static void main() {
        int x;
        
        System.out.println("Test 1: Before switch");
        x = 1;
        
        if (x == 1) {
            System.out.println("Test 1: x equals 1 (if statement)");
        }
        
        System.out.println("Test 2: Entering switch");
        switch (x) {
            case 1:
                System.out.println("Test 2: Case 1 matched");
                break;
            default:
                System.out.println("Test 2: Default case");
                break;
        }
        
        System.out.println("Test 3: After switch");
        return;
    }
}