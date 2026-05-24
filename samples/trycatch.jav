class TryCatchTest {
    public static void main() {
        int result = 0;
        
        // Test basic try-catch
        try {
            result = 1;
        } catch (Exception e) {
            result = 0;
        }
        
        // Test try-catch-finally
        int finallyRan = 0;
        try {
            result = result + 1;
        } catch (Exception e) {
            result = 0;
        } finally {
            finallyRan = 1;
        }
        
        // Test catch with exception
        int caught = 0;
        try {
            throw new Exception("Test exception");
        } catch (Exception e) {
            caught = 1;
        }
        
        // Test finally runs even without exception
        int finallyRan2 = 0;
        try {
            result = result + 1;
        } finally {
            finallyRan2 = 1;
        }
        
        // Verify results: result=3, finallyRan=1, caught=1, finallyRan2=1
        if (result == 3 && finallyRan == 1 && caught == 1 && finallyRan2 == 1) {
            System.out.println("trycatch.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
