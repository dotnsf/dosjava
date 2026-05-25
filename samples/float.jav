class FloatTest {
    public static void main() {
        // Test float variable declaration and assignment
        float a = 3.14f;
        float b = 2.5f;
        
        // Test basic arithmetic
        float sum = 1.5f + 2.5f;
        float diff = 5.0f - 2.0f;
        float prod = 2.0f * 3.0f;
        float quot = 10.0f / 2.0f;
        
        // Test comparison (with tolerance for floating point)
        boolean eq = (sum > 3.99f && sum < 4.01f);  // ~4.0
        boolean lt = (a < b);
        boolean gt = (b > a);
        
        // Test negative values
        float neg = -1.5f;

        // Verify results
        if (eq && !lt && !gt &&
            diff > 2.99f && diff < 3.01f &&
            prod > 5.99f && prod < 6.01f &&
            quot > 4.99f && quot < 5.01f &&
            neg < 0.0f) {
            System.out.println("float.jav worked correctly.");
        }
        
        return;
    }
}
