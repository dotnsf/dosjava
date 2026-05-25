class TryUpdateTest {
    void main() {
        int result = 0;
        
        System.out.println("Initial: " + result);
        
        // Test 1: Simple try block
        try {
            result = 1;
        } catch (Exception e) {
            result = 0;
        }
        System.out.println("After try 1: " + result);
        
        // Test 2: Try with addition
        try {
            result = result + 1;
        } catch (Exception e) {
            result = 0;
        }
        System.out.println("After try 2: " + result);
        
        // Test 3: Try with finally
        try {
            result = result + 1;
        } finally {
            System.out.println("In finally, result: " + result);
        }
        System.out.println("After try 3: " + result);
    }
}
