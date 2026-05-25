class ThrowTest {
    public static void main() {
        int result = 0;
        
        // Test throw and catch
        try {
            result = 1;
            throw 100;
            result = 0;  // Should not execute
        } catch (Exception e) {
            result = 2;
        }
        
        // Test another throw and catch
        int test2 = 0;
        try {
            test2 = 1;
            throw 200;
        } catch (Exception e) {
            test2 = 3;
        }
        
        // Verify results: result=2, test2=3
        if (result == 2 && test2 == 3) {
            System.out.println("throw.jav worked correctly.");
        }
        
        return;
    }
}
