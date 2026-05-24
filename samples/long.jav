class LongTest {
    public static void main() {
        // Test long variable declaration and assignment
        long a = 1000000000L;
        long b = 2000000000L;
        long c = a + b;
        
        // Test basic arithmetic
        long sum = 100L + 200L;
        long diff = 500L - 300L;
        long prod = 10L * 20L;
        long quot = 1000L / 10L;
        long rem = 1007L % 100L;
        
        // Test comparison
        boolean eq = (a == 1000000000L);
        boolean lt = (a < b);
        boolean gt = (b > a);
        
        // Test large values
        long large = 9223372036854775807L;  // Max long value
        
        // Verify results
        if (c == 3000000000L && sum == 300L && diff == 200L && 
            prod == 200L && quot == 100L && rem == 7L &&
            eq && lt && gt) {
            System.out.println("long.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
