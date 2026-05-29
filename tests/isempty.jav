// Test String.isEmpty() method
class IsEmptyTest {
    public static void main() {
        testEmpty();
        testNonEmpty();
        System.out.println("isEmpty tests completed!");
    }
    
    static void testEmpty() {
        String s1 = "";
        boolean b1 = s1.isEmpty();
        
        System.out.println("Empty string isEmpty(): ");
        System.out.println(b1);  // Should be 1 (true)
        
        if (b1) {
            System.out.println("PASS: Empty string detected");
        }
    }
    
    static void testNonEmpty() {
        String s2 = "Hello";
        boolean b2 = s2.isEmpty();
        
        System.out.println("Non-empty string isEmpty(): ");
        System.out.println(b2);  // Should be 0 (false)
        
        if (!b2) {
            System.out.println("PASS: Non-empty string detected");
        }
        
        String s3 = " ";  // Single space
        boolean b3 = s3.isEmpty();
        
        System.out.println("Single space isEmpty(): ");
        System.out.println(b3);  // Should be 0 (false)
        
        if (!b3) {
            System.out.println("PASS: Space is not empty");
        }
    }
}