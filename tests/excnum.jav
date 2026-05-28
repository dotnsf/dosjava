class ExcNum {
    public static void main(String[] args) {
        System.out.println("Testing NumberFormatException...");
        
        // Test 1: Valid integer parsing
        try {
            int x = Integer.parseInt("123");
            System.out.println("Test 1 passed: Parsed 123");
        } catch (Exception e) {
            System.out.println("ERROR: Should not have thrown exception");
        }
        
        // Test 2: Invalid integer (letters)
        try {
            int x = Integer.parseInt("abc");
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 2 passed: Caught invalid integer");
        }
        
        // Test 3: Invalid integer (mixed)
        try {
            int x = Integer.parseInt("12a34");
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 3 passed: Caught mixed characters");
        }
        
        // Test 4: Empty string
        try {
            int x = Integer.parseInt("");
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 4 passed: Caught empty string");
        }
        
        // Test 5: Negative number
        try {
            int x = Integer.parseInt("-42");
            System.out.println("Test 5 passed: Parsed -42");
        } catch (Exception e) {
            System.out.println("ERROR: Should not have thrown exception");
        }
        
        // Test 6: Positive sign
        try {
            int x = Integer.parseInt("+99");
            System.out.println("Test 6 passed: Parsed +99");
        } catch (Exception e) {
            System.out.println("ERROR: Should not have thrown exception");
        }
        
        System.out.println("excnum.jav worked correctly.");
    }
}

