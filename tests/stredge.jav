// String Extension Edge Cases Test
// Tests edge cases and boundary conditions

class StringEdgeTest {
    public static void main() {
        System.out.println("=== String Edge Cases Test ===");
        System.out.println("");
        
        testEmptyStrings();
        testLongStrings();
        testSpecialCharacters();
        testBoundaryConditions();
        
        System.out.println("");
        System.out.println("=== Edge Case Tests Completed ===");
    }
    
    // Test with empty strings
    public static void testEmptyStrings() {
        System.out.println("--- Test: Empty Strings ---");
        
        String empty = "";
        
        // isEmpty
        int e = empty.isEmpty();
        System.out.println("Empty isEmpty: ");
        System.out.println(e);
        
        // trim
        String t = empty.trim();
        System.out.println("Empty trim length: ");
        System.out.println(t.length());
        
        // replace
        String r = empty.replace("x", "y");
        System.out.println("Empty replace length: ");
        System.out.println(r.length());
        
        // contains
        int c1 = empty.contains("");
        System.out.println("Empty contains empty: ");
        System.out.println(c1);
        
        int c2 = empty.contains("x");
        System.out.println("Empty contains 'x': ");
        System.out.println(c2);
        
        // repeat
        String rep = empty.repeat(5);
        System.out.println("Empty repeat(5) length: ");
        System.out.println(rep.length());
        
        System.out.println("");
    }
    
    // Test with longer strings
    public static void testLongStrings() {
        System.out.println("--- Test: Long Strings ---");
        
        String long1 = "This is a longer test string for testing";
        
        // lastIndexOf
        int idx = long1.lastIndexOf("test");
        System.out.println("Last 'test' at: ");
        System.out.println(idx);
        
        // contains
        int c = long1.contains("longer");
        System.out.println("Contains 'longer': ");
        System.out.println(c);
        
        // replace
        String r = long1.replace("test", "demo");
        System.out.println("After replace: ");
        System.out.println(r);
        
        System.out.println("");
    }
    
    // Test with special characters
    public static void testSpecialCharacters() {
        System.out.println("--- Test: Special Characters ---");
        
        // Spaces and tabs
        String s1 = "  \t  Hello  \t  ";
        String t1 = s1.trim();
        System.out.println("Trim tabs: [");
        System.out.println(t1);
        System.out.println("]");
        
        // Multiple spaces
        String s2 = "a  b  c";
        String r2 = s2.replace("  ", "-");
        System.out.println("Replace double space: ");
        System.out.println(r2);
        
        System.out.println("");
    }
    
    // Test boundary conditions
    public static void testBoundaryConditions() {
        System.out.println("--- Test: Boundary Conditions ---");
        
        String s = "abc";
        
        // charAt at boundaries
        String c1 = s.charAt(0);
        System.out.println("First char: ");
        System.out.println(c1);
        
        String c2 = s.charAt(2);
        System.out.println("Last char: ");
        System.out.println(c2);
        
        // lastIndexOf at start
        int idx1 = s.lastIndexOf("a");
        System.out.println("'a' at: ");
        System.out.println(idx1);
        
        // lastIndexOf at end
        int idx2 = s.lastIndexOf("c");
        System.out.println("'c' at: ");
        System.out.println(idx2);
        
        // repeat 0 times
        String r0 = s.repeat(0);
        System.out.println("repeat(0) length: ");
        System.out.println(r0.length());
        
        // repeat 1 time
        String r1 = s.repeat(1);
        System.out.println("repeat(1): ");
        System.out.println(r1);
        
        // compareTo with same string
        int cmp = s.compareTo("abc");
        System.out.println("Compare equal: ");
        System.out.println(cmp);
        
        System.out.println("");
    }
}