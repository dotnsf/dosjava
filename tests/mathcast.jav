class MathCastTest {
    public static void main() {
        float f;
        int i;
        
        // Test Math.floor with cast
        f = 3.7f;
        i = (int)Math.floor(f);
        System.out.println(i);
        
        // Test Math.ceil with cast
        f = 3.2f;
        i = (int)Math.ceil(f);
        System.out.println(i);
        
        // Test negative values
        f = -2.3f;
        i = (int)Math.floor(f);
        System.out.println(i);
        
        f = -2.7f;
        i = (int)Math.ceil(f);
        System.out.println(i);
    }
}
