/**
 * Nested Try-Catch Tests
 */
class TestNested {
    public static void main(String[] args) {
        System.out.println("=== Nested Tests ===");
        
        test1_nested_try_catch();
        test2_inner_exception_caught();
        
        System.out.println("=== Tests Done ===");
    }
    
    /**
     * Test 1: Basic nested try-catch
     */
    public static void test1_nested_try_catch() {
        System.out.println("Test 1: Nested");
        int outer_status;
        int inner_status;
        
        try {
            System.out.println("Outer try");
            outer_status = 1;
            
            try {
                System.out.println("Inner try");
                inner_status = 1;
            } catch (Exception e) {
                System.out.println("Inner catch");
                inner_status = 2;
            }
            
            System.out.println("After inner");
            outer_status = 2;
        } catch (Exception e) {
            System.out.println("Outer catch");
            outer_status = 3;
        }
        
        System.out.println("Outer: " + outer_status);
        System.out.println("Inner: " + inner_status);
    }
    
    /**
     * Test 2: Exception in inner block
     */
    public static void test2_inner_exception_caught() {
        System.out.println("Test 2: Inner exc");
        int result;
        
        try {
            System.out.println("Outer try");
            result = 10;
            
            try {
                System.out.println("Inner try");
                result = 20;
                throw new Exception();
            } catch (Exception e) {
                System.out.println("Inner catch");
                result = 40;
            }
            
            System.out.println("After inner");
            result = 50;
        } catch (Exception e) {
            System.out.println("Outer catch");
            result = 60;
        }
        
        System.out.println("Result: " + result);
    }
}
