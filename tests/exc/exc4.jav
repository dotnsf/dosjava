/**
 * Finally Block Tests
 */
class TestFinally {
    public static void main(String[] args) {
        System.out.println("=== Finally Tests ===");
        
        test1_finally_without_exception();
        test2_finally_with_exception();
        
        System.out.println("=== Tests Done ===");
    }
    
    /**
     * Test 1: Finally without exception
     */
    public static void test1_finally_without_exception() {
        System.out.println("Test 1: No exception");
        int try_executed;
        int finally_executed;
        
        try {
            System.out.println("Try block");
            try_executed = 1;
        } finally {
            System.out.println("Finally block");
            finally_executed = 1;
        }
        
        System.out.println("Try: " + try_executed);
        System.out.println("Finally: " + finally_executed);
    }
    
    /**
     * Test 2: Finally with exception
     */
    public static void test2_finally_with_exception() {
        System.out.println("Test 2: With exception");
        int try_executed;
        int catch_executed;
        int finally_executed;
        
        try {
            System.out.println("Try block");
            try_executed = 1;
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Catch block");
            catch_executed = 1;
        } finally {
            System.out.println("Finally block");
            finally_executed = 1;
        }
        
        System.out.println("Try: " + try_executed);
        System.out.println("Catch: " + catch_executed);
        System.out.println("Finally: " + finally_executed);
    }
}

// Made with Bob
