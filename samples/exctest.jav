// Exception handling with line numbers test (Phase 12)
class ExceptionTest {
    public static void main() {
        boolean allPassed = true;
        
        // Test 1: NullPointerException with line number
        allPassed = allPassed && testNullPointer();
        
        // Test 2: ArrayIndexOutOfBoundsException with line number
        allPassed = allPassed && testArrayBounds();
        
        // Test 3: NumberFormatException with line number
        allPassed = allPassed && testNumberFormat();
        
        // Test 4: IllegalArgumentException with line number
        allPassed = allPassed && testIllegalArgument();
        
        // Test 5: StringIndexOutOfBoundsException with line number
        allPassed = allPassed && testStringBounds();
        
        // Test 6: ArithmeticException with line number
        allPassed = allPassed && testArithmetic();
        
        if (allPassed) {
            System.out.println("exctest.jav worked correctly.");
        }
    }
    
    static boolean testNullPointer() {
        try {
            int[] arr;
            arr = null;
            int x = arr[0];  // Line 33: Should throw NullPointerException
            return false;
        } catch (Exception e) {
            String msg = e.getMessage();
            // Check if message contains "line" (行番号が含まれているか確認)
            return msg.indexOf("line") >= 0;
        }
    }
    
    static boolean testArrayBounds() {
        try {
            int[] arr;
            arr = new int[5];
            arr[10] = 20;  // Line 46: Should throw ArrayIndexOutOfBoundsException
            return false;
        } catch (Exception e) {
            String msg = e.getMessage();
            // Check if message contains "line" (行番号が含まれているか確認)
            return msg.indexOf("line") >= 0;
        }
    }
    
    static boolean testNumberFormat() {
        try {
            String s = "abc123";
            int n = Integer.parseInt(s);  // Line 56: Should throw NumberFormatException
            return false;
        } catch (Exception e) {
            String msg = e.getMessage();
            // Check if message contains "line" (行番号が含まれているか確認)
            return msg.indexOf("line") >= 0;
        }
    }
    
    static boolean testIllegalArgument() {
        try {
            String s = "test";
            String sub = s.substr(-1, 2);  // Line 67: Should throw IllegalArgumentException
            return false;
        } catch (Exception e) {
            String msg = e.getMessage();
            // Check if message contains "line" (行番号が含まれているか確認)
            return msg.indexOf("line") >= 0;
        }
    }
    
    static boolean testStringBounds() {
        try {
            String s = "hello";
            String sub = s.substr(0, 10);  // Line 78: Should throw StringIndexOutOfBoundsException
            return false;
        } catch (Exception e) {
            String msg = e.getMessage();
            // Check if message contains "line" (行番号が含まれているか確認)
            return msg.indexOf("line") >= 0;
        }
    }
    
    static boolean testArithmetic() {
        try {
            int a = 10;
            int b = 0;
            int c = a / b;  // Line 91: Should throw ArithmeticException
            return false;
        } catch (Exception e) {
            String msg = e.getMessage();
            // Check if message contains "line" (行番号が含まれているか確認)
            return msg.indexOf("line") >= 0;
        }
    }
}
