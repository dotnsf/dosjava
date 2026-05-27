/**
 * Line Number Test
 * Tests that error messages include source line numbers
 */
class LineNumberTest {
    public static void main() {
        System.out.println("=== Line Number Tests ===");
        
        test1_division_by_zero();
        test2_file_not_found();
        
        System.out.println("=== All Tests Complete ===");
    }
    
    /**
     * Test 1: Division by zero
     * Expected: Exception: Division by zero (line 22)
     */
    public static void test1_division_by_zero() {
        System.out.println("\nTest 1: Division by zero");
        try {
            int a = 10;
            int b = 0;
            int c = a / b;  // Line 22 - Error should occur here
        } catch (Exception e) {
            System.out.println(e);
        }
    }
    
    /**
     * Test 2: File not found
     * Expected: Exception: Cannot open file: missing.txt (line 35)
     */
    public static void test2_file_not_found() {
        System.out.println("\nTest 2: File not found");
        try {
            String line;
            File.open("missing.txt");  // Line 35 - Error should occur here
            line = File.readLine();
            System.out.println(line);
            File.close();
        } catch (Exception e) {
            System.out.println(e);
        }
    }
}
