class FloatAndTest {
    public static void main() {
        float zero = 0.0f;
        
        System.out.println("zero = " + zero);
        
        // Test individual conditions
        boolean cond1 = (zero > -0.01f);
        boolean cond2 = (zero < 0.01f);
        
        if (cond1) {
            System.out.println("zero > -0.01f: true");
        }
        if (cond2) {
            System.out.println("zero < 0.01f: true");
        }
        
        // Test combined condition
        boolean check = (zero > -0.01f && zero < 0.01f);
        
        if (check) {
            System.out.println("Combined check: true");
        } else {
            System.out.println("Combined check: false");
        }
        
        return;
    }
}
