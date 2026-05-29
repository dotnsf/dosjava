class StringTest {
    public static void main() {
        // Test string literals
        String s1 = "Hello";
        String s2 = "World";
        
        // Test string concatenation
        String s3 = s1 + " " + s2;
        
        // Test string comparison
        String s4 = "Hello";
        int eq1 = s1.equals(s4);    // 1 (true)
        int eq2 = s1.equals(s2);    // 0 (false)
        
        // Test string with numbers
        String s5 = "Value: " + 42;
        
        // Test empty string
        String empty = "";
        int isEmpty = empty.equals("");
        
        // Verify results
        if (s3.equals("Hello World") == 1 && eq1 == 1 && eq2 == 0 && isEmpty == 1) {
            System.out.println("string.jav worked correctly.");
        }
        
        return;
    }
}
