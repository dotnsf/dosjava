class StringExtTest {
    public static void main() {
        // Test all 8 new String methods
        boolean allPassed = true;
        
        // Test 1: charAt
        String s1 = "Hello";
        String c1 = s1.charAt(1);
        if (c1.equals("e") == 0) {
            allPassed = false;
        }
        
        // Test 2: isEmpty
        String empty = "";
        String notEmpty = "x";
        if (empty.isEmpty() != 1 || notEmpty.isEmpty() != 0) {
            allPassed = false;
        }
        
        // Test 3: trim
        String s2 = "  test  ";
        String t2 = s2.trim();
        if (t2.equals("test") == 0) {
            allPassed = false;
        }
        
        // Test 4: replace
        String s3 = "hello world";
        String r3 = s3.replace("world", "java");
        if (r3.equals("hello java") == 0) {
            allPassed = false;
        }
        
        // Test 5: compareTo
        String s4a = "abc";
        String s4b = "abc";
        String s4c = "xyz";
        if (s4a.compareTo(s4b) != 0 || s4a.compareTo(s4c) >= 0) {
            allPassed = false;
        }
        
        // Test 6: lastIndexOf
        String s5 = "test test";
        int idx5 = s5.lastIndexOf("test");
        if (idx5 != 5) {
            allPassed = false;
        }
        
        // Test 7: contains
        String s6 = "hello world";
        if (s6.contains("world") != 1 || s6.contains("xyz") != 0) {
            allPassed = false;
        }
        
        // Test 8: repeat
        String s7 = "ab";
        String r7 = s7.repeat(3);
        if (r7.equals("ababab") == 0) {
            allPassed = false;
        }
        
        // Test exception handling
        try {
            String s8 = "abc";
            String c8 = s8.charAt(10);
            allPassed = false;
        } catch (Exception e) {
            // Expected exception
        }
        
        try {
            String s9 = "x";
            String r9 = s9.repeat(-1);
            allPassed = false;
        } catch (Exception e) {
            // Expected exception
        }
        
        if (allPassed) {
            System.out.println("strext.jav worked correctly.");
        }
    }
}