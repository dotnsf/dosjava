/**
 * Edge Case Tests
 */
class TestEdge {
    public static void main(String[] args) {
        System.out.println("=== Edge Tests ===");
        
        test1_empty_blocks();
        test2_multiple_catches();
        
        System.out.println("=== Tests Done ===");
    }
    
    /**
     * Test 1: Empty blocks
     */
    public static void test1_empty_blocks() {
        System.out.println("Test 1: Empty blocks");
        int status;
        
        try {
            status = 1;
        } catch (Exception e) {
        }
        
        System.out.println("Status: " + status);
    }
    
    /**
     * Test 2: Multiple sequential catches
     */
    public static void test2_multiple_catches() {
        System.out.println("Test 2: Multiple catches");
        int count;
        
        count = 0;
        
        try {
            count = 1;
            throw new Exception();
        } catch (Exception e) {
            count = 2;
        }
        
        System.out.println("First: " + count);
        
        try {
            count = 3;
            throw new Exception();
        } catch (Exception e) {
            count = 4;
        }
        
        System.out.println("Second: " + count);
    }
}
