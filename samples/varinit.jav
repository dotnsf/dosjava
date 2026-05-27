class VarInitTest {
    public static void main() {
        // Test variable declaration with initialization
        // Phase 10.1 feature
        
        // Test primitive types with literal initialization
        int a = 42;
        long b = 123456L;
        float c = 3.14f;
        boolean d = true;
        
        // Test String initialization
        String e = "Hello";
        
        // Test expression initialization
        int f = 10 + 20;
        int g = a;
        int h = 2 * 36;
        
        // Test boolean expression initialization
        boolean i = (a == 42);
        
        // Verify all values are correct
        boolean allCorrect = true;
        
        if (a != 42) {
            allCorrect = false;
        }
        if (b != 123456L) {
            allCorrect = false;
        }
        if (c < 3.13f || c > 3.15f) {
            allCorrect = false;
        }
        if (d != true) {
            allCorrect = false;
        }
        if (f != 30) {
            allCorrect = false;
        }
        if (g != 42) {
            allCorrect = false;
        }
        if (h != 72) {
            allCorrect = false;
        }
        if (i != true) {
            allCorrect = false;
        }
        
        // Print success message if all tests passed
        if (allCorrect) {
            System.out.println("varinit.jav worked correctly.");
        }
        
        return;
    }
}
