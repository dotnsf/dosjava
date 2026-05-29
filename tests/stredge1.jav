// String Extension Edge Cases Test - Part 1
// Tests empty strings and long strings

class StringEdgeTest1 {
    public static void main() {
        System.out.println("=== String Edge Cases Test Part 1 ===");
        System.out.println("");
        
        testEmptyStrings();
        testLongStrings();
        
        System.out.println("");
        System.out.println("=== Part 1 Completed ===");
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
        System.out.println("Empty contains x: ");
        System.out.println(c2);
        
        // repeat
        String rep = empty.repeat(5);
        System.out.println("Empty repeat 5 length: ");
        System.out.println(rep.length());
        
        System.out.println("");
    }
    
    // Test with longer strings
    public static void testLongStrings() {
        System.out.println("--- Test: Long Strings ---");
        
        String long1 = "This is a longer test string for testing";
        
        // lastIndexOf
        int idx = long1.lastIndexOf("test");
        System.out.println("Last test at: ");
        System.out.println(idx);
        
        // contains
        int c = long1.contains("longer");
        System.out.println("Contains longer: ");
        System.out.println(c);
        
        // replace
        String r = long1.replace("test", "demo");
        System.out.println("After replace: ");
        System.out.println(r);
        
        System.out.println("");
    }
}