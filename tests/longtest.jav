class LongTest {
    public static void main() {
        // Small long value (fits in 16-bit)
        long a = 100L;
        System.out.println(a);
        
        // Medium long value (requires 32-bit)
        long b = 65536L;
        System.out.println(b);
        
        // Large long value
        long c = 123456L;
        System.out.println(c);
        
        // Negative long value
        long d = -1000L;
        System.out.println(d);
        
        System.out.println("Done");
    }
}
