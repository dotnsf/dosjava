class CastTest {
    public static void main() {
        int i;
        long l;
        float f;
        
        // Test int to long (widening)
        i = 42;
        l = (long)i;
        System.out.println(l);
        
        // Test int to float (widening)
        i = 100;
        f = (float)i;
        System.out.println(f);
        
        // Test long to int (narrowing)
        l = 1000L;
        i = (int)l;
        System.out.println(i);
        
        // Test float to int (narrowing)
        f = 3.14f;
        i = (int)f;
        System.out.println(i);
    }
}
