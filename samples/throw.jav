class ThrowTest {
    public static void main() {
        int result = 0;
        
        // Test throw and catch
        try {
            result = 1;
            throw new Exception("Test message");
            result = 0;  // Should not execute
        } catch (Exception e) {
            result = 2;
        }
        
        // Test throw with message retrieval
        String msg = "";
        try {
            throw new Exception("Error occurred");
        } catch (Exception e) {
            msg = e.getMessage();
        }
        
        // Test nested try-catch with throw
        int nested = 0;
        try {
            try {
                throw new Exception("Inner");
            } catch (Exception e) {
                nested = 1;
                throw new Exception("Outer");
            }
        } catch (Exception e) {
            nested = 2;
        }
        
        // Verify results: result=2, msg="Error occurred", nested=2
        if (result == 2 && msg.equals("Error occurred") && nested == 2) {
            System.out.println("throw.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
