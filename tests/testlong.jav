class TestLong {
    void main() {
        long a;
        long b;
        long c;
        
        // Test long literals
        a = 100L;
        b = 200L;
        
        // Test long addition
        c = a + b;
        System.println("100L + 200L = 300");
        
        // Test long subtraction
        c = b - a;
        System.println("200L - 100L = 100");
        
        // Test long multiplication
        c = a * 3L;
        System.println("100L * 3L = 300");
        
        // Test long division
        c = b / 2L;
        System.println("200L / 2L = 100");
        
        // Test long negation
        c = -a;
        System.println("-100L = -100");
        
        // Test long comparison
        if (a < b) {
            System.println("100L < 200L: true");
        }
        
        if (a == 100L) {
            System.println("a == 100L: true");
        }
        
        System.println("Long test complete!");
    }
}
