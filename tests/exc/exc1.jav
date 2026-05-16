/**
 * Basic Exception Handling Tests
 * Tests fundamental try-catch-finally functionality
 */
class TestBasic {
    public static void main(String[] args) {
        System.out.println("=== Basic Exception Tests ===");
        
        test1_basic_try_catch();
        test2_try_finally_no_exception();
        test3_try_catch_finally_with_exception();
        
        System.out.println("=== All Basic Tests Completed ===");
    }
    
    /**
     * Test 1: Basic try-catch
     * Expected: Catch block executes when exception is thrown
     */
    public static void test1_basic_try_catch() {
        System.out.println("\nTest 1: Basic try-catch");
        int result;
        
        try {
            System.out.println("  In try block");
            result = 1;
            throw new Exception();
            result = 2;  // Should not execute
        } catch (Exception e) {
            System.out.println("  In catch block");
            result = 3;
        }
        
        System.out.println("  Result: " + result);
        System.out.println("  Expected: 3");
    }
    
    /**
     * Test 2: Try-finally without exception
     * Expected: Finally block executes even without exception
     */
    public static void test2_try_finally_no_exception() {
        System.out.println("\nTest 2: Try-finally (no exception)");
        int result;
        
        try {
            System.out.println("  In try block");
            result = 1;
        } finally {
            System.out.println("  In finally block");
            result = 2;
        }
        
        System.out.println("  Result: " + result);
        System.out.println("  Expected: 2");
    }
    
    /**
     * Test 3: Try-catch-finally with exception
     * Expected: Catch and finally both execute
     */
    public static void test3_try_catch_finally_with_exception() {
        System.out.println("\nTest 3: Try-catch-finally (with exception)");
        int result;
        
        try {
            System.out.println("  In try block");
            result = 1;
            throw new Exception();
        } catch (Exception e) {
            System.out.println("  In catch block");
            result = 2;
        } finally {
            System.out.println("  In finally block");
            result = 3;
        }
        
        System.out.println("  Result: " + result);
        System.out.println("  Expected: 3");
    }
}
