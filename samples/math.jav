class MathTest {
    public static void main() {
        // Test abs()
        float abs1 = Math.abs(-5.0f);
        float abs2 = Math.abs(-3.5f);
        
        // Test min()
        float min1 = Math.min(10.0f, 20.0f);
        float min2 = Math.min(3.5f, 2.5f);
        
        // Test max()
        float max1 = Math.max(10.0f, 20.0f);
        float max2 = Math.max(3.5f, 2.5f);
        
        // Test sqrt()
        float sqrt1 = Math.sqrt(16.0f);
        float sqrt2 = Math.sqrt(2.0f);
        
        // Test pow()
        float pow1 = Math.pow(2.0f, 3.0f);  // 8.0
        float pow2 = Math.pow(5.0f, 2.0f);  // 25.0
        
        // Verify results
        boolean check1 = (abs1 > 4.99f && abs1 < 5.01f && abs2 > 3.49f && abs2 < 3.51f);
        boolean check2 = (min1 > 9.99f && min1 < 10.01f && min2 > 2.49f && min2 < 2.51f);
        boolean check3 = (max1 > 19.99f && max1 < 20.01f && max2 > 3.49f && max2 < 3.51f);
        boolean check4 = (sqrt1 > 3.99f && sqrt1 < 4.01f);
        boolean check5 = (sqrt2 > 1.41f && sqrt2 < 1.42f);
        boolean check6 = (pow1 > 7.99f && pow1 < 8.01f);
        boolean check7 = (pow2 > 24.99f && pow2 < 25.01f);
        
        if (check1 && check2 && check3 && check4 && check5 && check6 && check7) {
            System.out.println("math.jav worked correctly.");
        }
        
        return;
    }
}
