class MathIntTest {
    static void main() {
        int i;
        long l;
        float f;
        boolean ok;
        
        // Test Math.abs with int
        i = -42;
        f = Math.abs(i);
        ok = (f > 41.99f && f < 42.01f);
        
        // Test Math.abs with long
        l = -999L;
        f = Math.abs(l);
        ok = ok && (f > 998.99f && f < 999.01f);
        
        // Test Math.sqrt with int
        i = 16;
        f = Math.sqrt(i);
        ok = ok && (f > 3.99f && f < 4.01f);
        
        // Test Math.min with int
        i = 10;
        f = Math.min(i, 20);
        ok = ok && (f > 9.99f && f < 10.01f);
        
        // Test Math.max with long
        l = 100L;
        f = Math.max(l, 50L);
        ok = ok && (f > 99.99f && f < 100.01f);
        
        // Test Math.pow with int
        i = 2;
        f = Math.pow(i, 3);
        ok = ok && (f > 7.99f && f < 8.01f);
        
        // Test Math.floor with int
        i = 5;
        f = Math.floor(i);
        ok = ok && (f > 4.99f && f < 5.01f);
        
        // Test Math.ceil with long
        l = 7L;
        f = Math.ceil(l);
        ok = ok && (f > 6.99f && f < 7.01f);
        
        if (ok) {
            System.out.println("mathint.jav worked correctly.");
        }
    }
}
