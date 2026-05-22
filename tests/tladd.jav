// Simple long addition test
class TestLongAdd {
    public static void main() {
        long a;
        long b;
        long sum;
        
        a = 1000000L;
        b = 2000000L;
        
        System.print("a = ");
        System.printLong(a);
        System.print("\n");
        
        System.print("b = ");
        System.printLong(b);
        System.print("\n");
        
        sum = a + b;
        
        System.print("sum = ");
        System.printLong(sum);
        System.print("\n");
        
        System.print("Expected: 3000000\n");
    }
}

// Made with Bob
