// Test String.charAt() method
class CharAtTest {
    public static void main() {
        testBasic();
        testBounds();
        System.out.println("charAt tests completed!");
    }
    
    static void testBasic() {
        String s = "Hello";
        
        // Test each character
        int c0 = s.charAt(0);  // 'H' = 72
        int c1 = s.charAt(1);  // 'e' = 101
        int c2 = s.charAt(2);  // 'l' = 108
        int c3 = s.charAt(3);  // 'l' = 108
        int c4 = s.charAt(4);  // 'o' = 111
        
        System.out.println("charAt(0): ");
        System.out.println(c0);
        System.out.println("charAt(1): ");
        System.out.println(c1);
        System.out.println("charAt(2): ");
        System.out.println(c2);
        System.out.println("charAt(3): ");
        System.out.println(c3);
        System.out.println("charAt(4): ");
        System.out.println(c4);
    }
    
    static void testBounds() {
        String s = "Test";
        
        try {
            int c = s.charAt(10);  // Out of bounds
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Caught exception: ");
            System.out.println(e.getMessage());
        }
    }
}