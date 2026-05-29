// String Extension Methods Test - Part 1
// Tests: charAt, isEmpty, trim

class StringExtTest1 {
    public static void main() {
        System.out.println("=== String Extension Test Part 1 ===");
        System.out.println("");
        
        testCharAt();
        testIsEmpty();
        testTrim();
        
        System.out.println("");
        System.out.println("=== Part 1 Completed ===");
    }
    
    public static void testCharAt() {
        System.out.println("--- Test 1: charAt() ---");
        String s = "Hello";
        
        String c1 = s.charAt(0);
        System.out.println("charAt(0) = ");
        System.out.println(c1);
        
        String c2 = s.charAt(4);
        System.out.println("charAt(4) = ");
        System.out.println(c2);
        
        String c3 = s.charAt(4);
        System.out.println("Last char = ");
        System.out.println(c3);
        
        System.out.println("");
    }
    
    public static void testIsEmpty() {
        System.out.println("--- Test 2: isEmpty() ---");
        
        String empty = "";
        String notEmpty = "Hello";
        
        int e1 = empty.isEmpty();
        System.out.println("Empty string isEmpty: ");
        System.out.println(e1);
        
        int e2 = notEmpty.isEmpty();
        System.out.println("Non-empty isEmpty: ");
        System.out.println(e2);
        
        System.out.println("");
    }
    
    public static void testTrim() {
        System.out.println("--- Test 3: trim() ---");
        
        String s1 = "  Hello  ";
        String trimmed1 = s1.trim();
        System.out.println("Trimmed: [");
        System.out.println(trimmed1);
        System.out.println("]");
        
        String s2 = "NoSpaces";
        String trimmed2 = s2.trim();
        System.out.println("No spaces: [");
        System.out.println(trimmed2);
        System.out.println("]");
        
        String s3 = "   ";
        String trimmed3 = s3.trim();
        System.out.println("All spaces: [");
        System.out.println(trimmed3);
        System.out.println("]");
        
        System.out.println("");
    }
}