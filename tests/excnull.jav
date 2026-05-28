class ExcNull {
    public static void main(String[] args) {
        System.out.println("Testing NullPointerException...");
        
        // Test 1: Null array length
        try {
            int[] arr = null;
            int len = arr.length;
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 1 passed: Caught null array length");
        }
        
        // Test 2: Null array read
        try {
            int[] arr = null;
            int x = arr[0];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 2 passed: Caught null array read");
        }
        
        // Test 3: Null array write
        try {
            int[] arr = null;
            arr[0] = 42;
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 3 passed: Caught null array write");
        }
        
        // Test 4: Null long array read
        try {
            long[] larr = null;
            long x = larr[0];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 4 passed: Caught null long array read");
        }
        
        // Test 5: Null float array read
        try {
            float[] farr = null;
            float x = farr[0];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 5 passed: Caught null float array read");
        }
        
        System.out.println("excnull.jav worked correctly.");
    }
}

