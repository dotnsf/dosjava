class ExcArg {
    public static void main(String[] args) {
        System.out.println("Testing IllegalArgumentException...");
        
        // Test 1: Negative array size
        try {
            int[] arr = new int[-5];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 1 passed: Caught negative array size");
        }
        
        // Test 2: Valid array size (should succeed)
        try {
            int[] arr = new int[10];
            System.out.println("Test 2 passed: Created array with size 10");
        } catch (Exception e) {
            System.out.println("ERROR: Should not have thrown exception");
        }
        
        // Test 3: Substring with negative start index
        try {
            String s = "Hello";
            String sub = s.substr(-1, 3);
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 3 passed: Caught negative start index");
        }
        
        // Test 4: Substring with end < start
        try {
            String s = "Hello";
            String sub = s.substr(3, 1);
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 4 passed: Caught invalid range (end < start)");
        }
        
        // Test 5: Valid substring (should succeed)
        try {
            String s = "Hello";
            String sub = s.substr(1, 4);
            System.out.println("Test 5 passed: Valid substring");
        } catch (Exception e) {
            System.out.println("ERROR: Should not have thrown exception");
        }
        
        // Test 6: Zero-size array (should succeed)
        try {
            int[] arr = new int[0];
            System.out.println("Test 6 passed: Created zero-size array");
        } catch (Exception e) {
            System.out.println("ERROR: Should not have thrown exception");
        }
        
        System.out.println("excarg.jav worked correctly.");
    }
}
