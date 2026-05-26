class MathIntTest {
    public static void main() {
        int i1 = -10;
        int i2 = 5;
        int i3 = 3;
        
        // Test Math.abs with int
        float abs_result = Math.abs(i1);
        System.out.println(abs_result);
        
        // Test Math.min with int
        float min_result = Math.min(i1, i2);
        System.out.println(min_result);
        
        // Test Math.max with int
        float max_result = Math.max(i1, i2);
        System.out.println(max_result);
        
        // Test Math.sqrt with int
        float sqrt_result = Math.sqrt(i3);
        System.out.println(sqrt_result);
    }
}
