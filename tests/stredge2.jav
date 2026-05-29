// String Extension Edge Cases Test - Part 2
// Tests special characters and boundary conditions

class StringEdgeTest2 {
    public static void main() {
        System.out.println("=== String Edge Cases Test Part 2 ===");
        System.out.println("");
        
        testSpecialCharacters();
        testBoundaryConditions();
        
        System.out.println("");
        System.out.println("=== Part 2 Completed ===");
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
        System.out.println("a at: ");
        System.out.println(idx1);
        
        // lastIndexOf at end
        int idx2 = s.lastIndexOf("c");
        System.out.println("c at: ");
        System.out.println(idx2);
        
        // repeat 0 times
        String r0 = s.repeat(0);
        System.out.println("repeat 0 length: ");
        System.out.println(r0.length());
        
        // repeat 1 time
        String r1 = s.repeat(1);
        System.out.println("repeat 1: ");
        System.out.println(r1);
        
        // compareTo with same string
        int cmp = s.compareTo("abc");
        System.out.println("Compare equal: ");
        System.out.println(cmp);
        
        System.out.println("");
    }
}