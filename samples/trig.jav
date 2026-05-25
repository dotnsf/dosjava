class TrigTest {
    public static void main() {
        float pi = 3.14159f;
        
        // Test sin()
        float sin0 = Math.sin(0.0f);
        float sin90 = Math.sin(pi / 2.0f);  // ~1.0
        
        // Test cos()
        float cos0 = Math.cos(0.0f);        // ~1.0
        float cos90 = Math.cos(pi / 2.0f);  // ~0.0
        
        // Test tan()
        float tan0 = Math.tan(0.0f);
        float tan45 = Math.tan(pi / 4.0f);  // ~1.0
        
        // Verify results (with tolerance for floating point)
        boolean check1 = (sin0 > -0.01f && sin0 < 0.01f);
        boolean check2 = (sin90 > 0.99f && sin90 < 1.01f);
        boolean check3 = (cos0 > 0.99f && cos0 < 1.01f);
        boolean check4 = (cos90 > -0.01f && cos90 < 0.01f);
        boolean check5 = (tan0 > -0.01f && tan0 < 0.01f);
        boolean check6 = (tan45 > 0.99f && tan45 < 1.01f);

        if (check1 && check2 && check3 && check4 && check5 && check6) {
            System.out.println("trig.jav worked correctly.");
        }
        
        return;
    }
}
