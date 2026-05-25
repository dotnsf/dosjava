class FAndDebug {
    void main() {
        float zero = 0.0f;
        float neg = -0.01f;
        
        System.out.println("zero = " + Float.toString(zero));
        System.out.println("neg = " + Float.toString(neg));
        
        // Test individual condition
        boolean cond1 = zero > neg;
        System.out.println("zero > neg: " + cond1);
    }
}
