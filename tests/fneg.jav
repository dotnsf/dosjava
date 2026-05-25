class FloatNegTest {
    public static void main() {
        float zero = 0.0f;
        float neg = -0.01f;
        float pos = 0.01f;
        
        System.out.println("zero = " + zero);
        System.out.println("neg = " + neg);
        System.out.println("pos = " + pos);
        
        boolean test1 = (zero > neg);
        boolean test2 = (zero < pos);
        
        if (test1) {
            System.out.println("zero > neg: true");
        }
        if (test2) {
            System.out.println("zero < pos: true");
        }
        
        if (test1 && test2) {
            System.out.println("Both tests passed!");
        }
        
        return;
    }
}
