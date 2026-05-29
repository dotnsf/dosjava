// String Extension Methods Test - Part 3
// Tests: lastIndexOf, contains, repeat

class StringExtTest3 {
    public static void main() {
        System.out.println("=== String Extension Test Part 3 ===");
        System.out.println("");
        
        testLastIndexOf();
        testContains();
        testRepeat();
        
        System.out.println("");
        System.out.println("=== Part 3 Completed ===");
    }
    
    public static void testLastIndexOf() {
        System.out.println("--- Test 6: lastIndexOf() ---");
        
        String s1 = "Hello Hello World";
        int idx1 = s1.lastIndexOf("Hello");
        System.out.println("Last Hello at: ");
        System.out.println(idx1);
        
        int idx2 = s1.lastIndexOf("World");
        System.out.println("Last World at: ");
        System.out.println(idx2);
        
        int idx3 = s1.lastIndexOf("test");
        System.out.println("Not found: ");
        System.out.println(idx3);
        
        System.out.println("");
    }
    
    public static void testContains() {
        System.out.println("--- Test 7: contains() ---");
        
        String s = "Hello World";
        
        int c1 = s.contains("World");
        System.out.println("Contains World: ");
        System.out.println(c1);
        
        int c2 = s.contains("Hello");
        System.out.println("Contains Hello: ");
        System.out.println(c2);
        
        int c3 = s.contains("test");
        System.out.println("Contains test: ");
        System.out.println(c3);
        
        System.out.println("");
    }
    
    public static void testRepeat() {
        System.out.println("--- Test 8: repeat() ---");
        
        String s1 = "-";
        String r1 = s1.repeat(10);
        System.out.println("Repeat - 10 times: ");
        System.out.println(r1);
        
        String s2 = "ab";
        String r2 = s2.repeat(3);
        System.out.println("Repeat ab 3 times: ");
        System.out.println(r2);
        
        String s3 = "x";
        String r3 = s3.repeat(0);
        System.out.println("Repeat 0 times: [");
        System.out.println(r3);
        System.out.println("]");
        
        System.out.println("");
    }
}