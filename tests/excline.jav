// Exception Line Number Test
// Tests all exception types with line number reporting

class ExceptionLineTest {
    static void main() {
        System.println("=== Exception Line Number Test ===");
        
        // Test 1: NullPointerException
        System.println("\n1. Testing NullPointerException:");
        testNullPointer();
        
        // Test 2: ArrayIndexOutOfBoundsException
        System.println("\n2. Testing ArrayIndexOutOfBoundsException:");
        testArrayBounds();
        
        // Test 3: NumberFormatException
        System.println("\n3. Testing NumberFormatException:");
        testNumberFormat();
        
        // Test 4: IllegalArgumentException
        System.println("\n4. Testing IllegalArgumentException:");
        testIllegalArgument();
        
        // Test 5: StringIndexOutOfBoundsException
        System.println("\n5. Testing StringIndexOutOfBounds:");
        testStringBounds();
        
        // Test 6: ArithmeticException (division by zero)
        System.println("\n6. Testing ArithmeticException:");
        testArithmetic();
        
        System.println("\n=== All Tests Completed ===");
    }
    
    // Test 1: NullPointerException
    static void testNullPointer() {
        try {
            int[] arr;
            int x;
            arr = null;
            x = arr[0];  // Should throw NullPointerException
            System.println("ERROR: Should not reach here");
        } catch (Exception e) {
            System.println("" + e);
        }
    }
    
    // Test 2: ArrayIndexOutOfBoundsException
    static void testArrayBounds() {
        try {
            int[] arr;
            arr = new int[5];
            arr[0] = 10;
            arr[10] = 20;  // Should throw ArrayIndexOutOfBoundsException
            System.println("ERROR: Should not reach here");
        } catch (Exception e) {
            System.println("" + e);
        }
    }
    
    // Test 3: NumberFormatException
    static void testNumberFormat() {
        try {
            String s;
            s = "abc123";
            int n;
            n = Integer.parseInt(s);  // Should throw NumberFormatException
            System.println("ERROR: Should not reach here");
        } catch (Exception e) {
            System.println("" + e);
        }
    }
    
    // Test 4: IllegalArgumentException
    static void testIllegalArgument() {
        try {
            String s;
            String sub;
            s = "test";
            sub = s.substr(-1, 2);  // Should throw IllegalArgumentException
            System.println("ERROR: Should not reach here");
        } catch (Exception e) {
            System.println("" + e);
        }
    }
    
    // Test 5: StringIndexOutOfBoundsException
    static void testStringBounds() {
        try {
            String s;
            String sub;
            s = "hello";
            sub = s.substr(0, 10);  // Should throw StringIndexOutOfBoundsException
            System.println("ERROR: Should not reach here");
        } catch (Exception e) {
            System.println("" + e);
        }
    }
    
    // Test 6: ArithmeticException (division by zero)
    static void testArithmetic() {
        try {
            int a;
            int b;
            int c;
            a = 10;
            b = 0;
            c = a / b;  // Should throw ArithmeticException
            System.println("ERROR: Should not reach here");
        } catch (Exception e) {
            System.println("" + e);
        }
    }
}
