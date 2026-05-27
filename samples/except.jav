/**
 * Exception Handling Sample
 * Demonstrates exception handling with error messages
 */
class ExceptionSample {
    public static void main() {
        boolean all_passed = true;
        
        // Test 1: Basic exception handling
        try {
            throw new Exception();
        } catch (Exception e) {
            // Exception caught successfully
        }
        
        // Test 2: Division by zero
        try {
            int a = 10;
            int b = 0;
            int c = a / b;
            all_passed = false;
        } catch (Exception e) {
            // Exception caught successfully
        }
        
        // Test 3: File not found
        try {
            File.open("nonexist.txt");
            all_passed = false;
        } catch (Exception e) {
            // Exception caught successfully
        }
        
        if (all_passed) {
            System.out.println("except.jav worked correctly.");
        }
    }
}

