// String Extension Methods Test
// Tests all 8 new String methods added in Phase 13

class StringExtTest {
    public static void main() {
        System.print("=== String Extension Methods Test ===");
        System.print("");
        
        testCharAt();
        testIsEmpty();
        testTrim();
        testReplace();
        testCompareTo();
        testLastIndexOf();
        testContains();
        testRepeat();
        
        System.print("");
        System.print("=== All Tests Completed ===");
    }
    
    // Test 1: charAt(int index)
    public static void testCharAt() {
        System.print("--- Test 1: charAt() ---");
        String s = "Hello";
        
        int c1 = s.charAt(0);
        System.print("charAt(0) = ");
        System.printInt(c1);
        
        int c2 = s.charAt(4);
        System.print("charAt(4) = ");
        System.printInt(c2);
        
        int c3 = s.charAt(4);
        System.print("Last char = ");
        System.printInt(c3);
        
        System.print("");
    }
    
    // Test 2: isEmpty()
    public static void testIsEmpty() {
        System.print("--- Test 2: isEmpty() ---");
        
        String empty = "";
        String notEmpty = "Hello";
        
        int e1 = empty.isEmpty();
        System.print("Empty string isEmpty: ");
        System.printInt(e1);
        
        int e2 = notEmpty.isEmpty();
        System.print("Non-empty isEmpty: ");
        System.printInt(e2);
        
        System.print("");
    }
    
    // Test 3: trim()
    public static void testTrim() {
        System.print("--- Test 3: trim() ---");
        
        String s1 = "  Hello  ";
        String trimmed1 = s1.trim();
        System.print("Trimmed: [");
        System.print(trimmed1);
        System.print("]");
        
        String s2 = "NoSpaces";
        String trimmed2 = s2.trim();
        System.print("No spaces: [");
        System.print(trimmed2);
        System.print("]");
        
        String s3 = "   ";
        String trimmed3 = s3.trim();
        System.print("All spaces: [");
        System.print(trimmed3);
        System.print("]");
        
        System.print("");
    }
    
    // Test 4: replace(String, String)
    public static void testReplace() {
        System.print("--- Test 4: replace() ---");
        
        String s1 = "Hello World";
        String r1 = s1.replace("World", "Java");
        System.print("Replace World->Java: ");
        System.print(r1);
        
        String s2 = "aaa bbb aaa";
        String r2 = s2.replace("aaa", "xxx");
        System.print("Replace aaa->xxx: ");
        System.print(r2);
        
        String s3 = "test";
        String r3 = s3.replace("x", "y");
        System.print("No match: ");
        System.print(r3);
        
        System.print("");
    }
    
    // Test 5: compareTo(String)
    public static void testCompareTo() {
        System.print("--- Test 5: compareTo() ---");
        
        String s1 = "apple";
        String s2 = "banana";
        String s3 = "apple";
        
        int cmp1 = s1.compareTo(s2);
        System.print("apple vs banana: ");
        System.printInt(cmp1);
        
        int cmp2 = s2.compareTo(s1);
        System.print("banana vs apple: ");
        System.printInt(cmp2);
        
        int cmp3 = s1.compareTo(s3);
        System.print("apple vs apple: ");
        System.printInt(cmp3);
        
        System.print("");
    }
    
    // Test 6: lastIndexOf(String)
    public static void testLastIndexOf() {
        System.print("--- Test 6: lastIndexOf() ---");
        
        String s1 = "Hello Hello World";
        int idx1 = s1.lastIndexOf("Hello");
        System.print("Last 'Hello' at: ");
        System.printInt(idx1);
        
        int idx2 = s1.lastIndexOf("World");
        System.print("Last 'World' at: ");
        System.printInt(idx2);
        
        int idx3 = s1.lastIndexOf("test");
        System.print("Not found: ");
        System.printInt(idx3);
        
        System.print("");
    }
    
    // Test 7: contains(String)
    public static void testContains() {
        System.print("--- Test 7: contains() ---");
        
        String s = "Hello World";
        
        int c1 = s.contains("World");
        System.print("Contains 'World': ");
        System.printInt(c1);
        
        int c2 = s.contains("Hello");
        System.print("Contains 'Hello': ");
        System.printInt(c2);
        
        int c3 = s.contains("test");
        System.print("Contains 'test': ");
        System.printInt(c3);
        
        System.print("");
    }
    
    // Test 8: repeat(int)
    public static void testRepeat() {
        System.print("--- Test 8: repeat() ---");
        
        String s1 = "-";
        String r1 = s1.repeat(10);
        System.print("Repeat '-' 10 times: ");
        System.print(r1);
        
        String s2 = "ab";
        String r2 = s2.repeat(3);
        System.print("Repeat 'ab' 3 times: ");
        System.print(r2);
        
        String s3 = "x";
        String r3 = s3.repeat(0);
        System.print("Repeat 0 times: [");
        System.print(r3);
        System.print("]");
        
        System.print("");
    }
}