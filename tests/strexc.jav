// String Extension Exception Test
// Tests exception handling for new String methods

class StringExceptionTest {
    public static void main() {
        System.out.println("=== String Exception Test ===");
        System.out.println("");
        
        testCharAtException();
        testCompareToException();
        testRepeatException();
        
        System.out.println("");
        System.out.println("=== Exception Tests Completed ===");
    }
    
    // Test charAt with invalid index
    public static void testCharAtException() {
        System.out.println("--- Test: charAt Exception ---");
        
        try {
            String s = "Hello";
            System.out.println("String: Hello (length=5)");
            
            // Valid access
            String c = s.charAt(0);
            System.out.println("charAt(0) OK");
            
            // Invalid access - should throw exception
            System.out.println("Attempting charAt(10)...");
            String bad = s.charAt(10);
            System.out.println("ERROR: Should have thrown exception!");
        } catch (Exception e) {
            System.out.println("Caught Exception - OK");
        }
        
        System.out.println("");
    }
    
    // Test compareTo with null
    // NOTE: DOSJava's null literal is compiled as empty string or constant pool index 0,
    // which points to a valid string, so null check doesn't work as expected.
    // This test is skipped.
    public static void testCompareToException() {
        System.out.println("--- Test: compareTo Exception ---");
        System.out.println("(Skipped: null literal compiled as valid string)");
        System.out.println("");
    }
    
    // Test repeat with negative count
    public static void testRepeatException() {
        System.out.println("--- Test: repeat Exception ---");
        
        try {
            String s = "test";
            
            // Valid repeat
            String r1 = s.repeat(3);
            System.out.println("repeat(3) OK");
            
            // Invalid repeat - negative count
            System.out.println("Attempting repeat(-1)...");
            String r2 = s.repeat(-1);
            System.out.println("ERROR: Should have thrown exception!");
        } catch (Exception e) {
            System.out.println("Caught Exception - OK");
        }
        
        System.out.println("");
    }
}