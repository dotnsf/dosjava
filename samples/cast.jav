class CastTest {
    static void main() {
        float f;
        int i;
        long l;
        boolean ok;
        
        // Test float to int cast
        f = 3.7f;
        i = (int)f;
        ok = (i == 3);
        
        // Test float to long cast
        f = 123.9f;
        l = (long)f;
        ok = ok && (l == 123L);
        
        // Test long to int cast
        l = 999L;
        i = (int)l;
        ok = ok && (i == 999);
        
        // Test int to float cast
        i = 42;
        f = (float)i;
        ok = ok && (f > 41.99f && f < 42.01f);
        
        // Test int to long cast
        i = 100;
        l = (long)i;
        ok = ok && (l == 100L);
        
        // Test with Math.floor
        f = 5.8f;
        i = (int)Math.floor(f);
        ok = ok && (i == 5);
        
        // Test with Math.ceil
        f = 5.2f;
        i = (int)Math.ceil(f);
        ok = ok && (i == 6);
        
        if (ok) {
            System.out.println("cast.jav worked correctly.");
        }
    }
}
