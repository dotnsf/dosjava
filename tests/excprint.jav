/**
 * Exception Variable Output Test
 * Tests printing Exception variables
 */
class ExceptionPrintTest {
    public static void main() {
        System.out.println("=== Exception Print Tests ===");
        
        test1_direct_println();
        test2_string_concatenation();
        test3_multiple_concatenations();

        test4_divided_by_zero();
        test5_read_non_existed_file();
        
        System.out.println("=== All Tests Complete ===");
    }
    
    /**
     * Test 1: Direct println of exception variable
     * Expected: Prints "Exception"
     */
    public static void test1_direct_println() {
        System.out.println("\nTest 1: Direct println");
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println(e);
        }
        System.out.println("Expected: Exception");
    }
    
    /**
     * Test 2: String concatenation with exception
     * Expected: Prints "Caught: Exception"
     */
    public static void test2_string_concatenation() {
        System.out.println("\nTest 2: String concatenation");
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Caught: " + e);
        }
        System.out.println("Expected: Caught: Exception");
    }
    
    /**
     * Test 3: Multiple concatenations
     * Expected: Prints "Error [Exception] occurred"
     */
    public static void test3_multiple_concatenations() {
        System.out.println("\nTest 3: Multiple concatenations");
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Error [" + e + "] occurred");
        }
        System.out.println("Expected: Error [Exception] occurred");
    }

    public static void test4_divided_by_zero() {
        System.out.println("\nTest 4: Divided by zero");
        try {
            int a = 10;
            int b = 0;
            int c = a / b;
        } catch (Exception e) {
            System.out.println("Exception: " + e);
        }
    }

    public static void test5_read_non_existed_file() {
        System.out.println("\nTest 5: Read non-existed file");
        try {
            String line1;
            File.open("test_.txt");
            line1 = File.readLine();
            System.out.println(line1);
            File.close();
        } catch (Exception e) {
            System.out.println("Exception: " + e);
        }
    }
}
