// String Extension Methods Test - Part 2
// Tests: replace, compareTo

class StringExtTest2 {
    public static void main() {
        System.out.println("=== String Extension Test Part 2 ===");
        System.out.println("");
        
        testReplace();
        testCompareTo();
        
        System.out.println("");
        System.out.println("=== Part 2 Completed ===");
    }
    
    public static void testReplace() {
        System.out.println("--- Test 4: replace() ---");
        
        String s1 = "Hello World";
        String r1 = s1.replace("World", "Java");
        System.out.println("Replace World->Java: ");
        System.out.println(r1);
        
        String s2 = "aaa bbb aaa";
        String r2 = s2.replace("aaa", "xxx");
        System.out.println("Replace aaa->xxx: ");
        System.out.println(r2);
        
        String s3 = "test";
        String r3 = s3.replace("x", "y");
        System.out.println("No match: ");
        System.out.println(r3);
        
        System.out.println("");
    }
    
    public static void testCompareTo() {
        System.out.println("--- Test 5: compareTo() ---");
        
        String s1 = "apple";
        String s2 = "banana";
        String s3 = "apple";
        
        int cmp1 = s1.compareTo(s2);
        System.out.println("apple vs banana: ");
        System.out.println(cmp1);
        
        int cmp2 = s2.compareTo(s1);
        System.out.println("banana vs apple: ");
        System.out.println(cmp2);
        
        int cmp3 = s1.compareTo(s3);
        System.out.println("apple vs apple: ");
        System.out.println(cmp3);
        
        System.out.println("");
    }
}