class IntTest {
    public static void main() {
        // Test int variable declaration and assignment
        int a = 10;
        int b = 20;
        int c = a + b;
        
        // Test basic arithmetic
        int sum = 5 + 3;
        int diff = 10 - 4;
        int prod = 6 * 7;
        int quot = 20 / 4;
        int rem = 17 % 5;
        
        // Verify results
        if (c == 30 && sum == 8 && diff == 6 && prod == 42 && quot == 5 && rem == 2) {
            System.out.println("int.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
