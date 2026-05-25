class StringTest {
    public static void main() {
        // Test string literals
        String s1 = "Hello";
        String s2 = "World";
        
        // Test string concatenation
        String s3 = s1 + " " + s2;
        
        // Test string comparison
        String s4 = "Hello";
        boolean eq1 = s1.equals(s4);    // true
        boolean eq2 = s1.equals(s2);    // false
        
        // Test string with numbers
        String s5 = "Value: " + 42;
        
        // Test empty string
        String empty = "";
        boolean isEmpty = empty.equals("");
        
        // Verify results
        if (s3.equals("Hello World") && eq1 && !eq2 && isEmpty) {
            System.out.println("string.jav worked correctly.");
        }
        
        return;
    }
}
