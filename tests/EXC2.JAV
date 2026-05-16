/**
 * Throw Statement Tests
 * Tests exception throwing behavior
 */
class TestThrow {
    public static void main(String[] args) {
        System.out.println("=== Throw Statement Tests ===");
        
        test1_basic_throw();
        test2_code_after_throw_not_executed();
        test3_catch_thrown_exception();
        
        System.out.println("=== All Throw Tests Completed ===");
    }
    
    /**
     * Test 1: Basic throw operation
     * Expected: Exception is thrown and caught
     */
    public static void test1_basic_throw() {
        System.out.println("\nTest 1: Basic throw");
        int status;
        
        try {
            System.out.println("  Before throw");
            status = 1;
            throw new Exception();
            status = 2;  // Should not execute
        } catch (Exception e) {
            System.out.println("  Exception caught");
            status = 3;
        }
        
        System.out.println("  Status: " + status);
        System.out.println("  Expected: 3");
    }
    
    /**
     * Test 2: Code after throw is not executed
     * Expected: Only code before throw executes
     */
    public static void test2_code_after_throw_not_executed() {
        System.out.println("\nTest 2: Code after throw not executed");
        int counter;
        
        try {
            counter = 0;
            System.out.println("  Counter: " + counter);
            counter = 1;
            System.out.println("  Counter: " + counter);
            throw new Exception();
            counter = 2;  // Should not execute
            System.out.println("  Counter: " + counter);  // Should not execute
        } catch (Exception e) {
            System.out.println("  Exception caught, counter: " + counter);
        }
        
        System.out.println("  Final counter: " + counter);
        System.out.println("  Expected: 1");
    }
    
    /**
     * Test 3: Catch handles thrown exception
     * Expected: Catch block receives and handles exception
     */
    public static void test3_catch_thrown_exception() {
        System.out.println("\nTest 3: Catch handles exception");
        int result;
        
        try {
            System.out.println("  Throwing exception");
            result = 10;
            throw new Exception();
        } catch (Exception e) {
            System.out.println("  Handling exception");
            result = 20;
        }
        
        System.out.println("  Result: " + result);
        System.out.println("  Expected: 20");
    }
}
