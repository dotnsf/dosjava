class ExcStr {
    public static void main(String[] args) {
        System.out.println("Testing StringIndexOutOfBoundsException...");
        
        // Test 1: substr with start index > length
        try {
            String s = "hello";
            String sub = s.substr(10);
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 1 passed: Caught start > length");
        }
        
        // Test 2: substr with end index > length
        try {
            String s = "hello";
            String sub = s.substr(0, 10);
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 2 passed: Caught end > length");
        }
        
        // Test 3: substr with start == length (boundary)
        try {
            String s = "hello";
            String sub = s.substr(5);
            System.out.println("Test 3 passed: Empty string result");
        } catch (Exception e) {
            System.out.println("ERROR: Should not throw exception");
        }
        
        // Test 4: substr with start = 0, end > length
        try {
            String s = "test";
            String sub = s.substr(0, 10);
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 4 passed: Caught end > length");
        }
        
        // Test 5: Valid substr at boundary
        try {
            String s = "hello";
            String sub = s.substr(0, 5);
            System.out.println("Test 5 passed: Full string result");
        } catch (Exception e) {
            System.out.println("ERROR: Should not throw exception");
        }
        
        // Test 6: Valid substr in middle
        try {
            String s = "hello";
            String sub = s.substr(1, 4);
            System.out.println("Test 6 passed: Middle substring result");
        } catch (Exception e) {
            System.out.println("ERROR: Should not throw exception");
        }
        
        System.out.println("All StringIndexOutOfBoundsException tests completed");
    }
}
