class LongTest {
    public static void main() {
        // Test long variable declaration and assignment
        long a = 100L;
        long b = 200L;
        long c = a + b;
        
        // Test basic arithmetic
        long sum = 50L + 30L;
        long diff = 100L - 40L;
        long prod = 60L * 70L;
        long quot = 200L / 40L;
        long rem = 170L % 50L;
        
        // Verify results
        if (c == 300L && sum == 80L && diff == 60L && prod == 4200L && quot == 5L && rem == 20L) {
            System.out.println("long.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
