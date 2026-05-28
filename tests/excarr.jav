class ExcArr {
    public static void main(String[] args) {
        System.out.println("Testing ArrayIndexOutOfBoundsException...");
        
        // Test 1: Index too large
        try {
            int[] arr = new int[5];
            int x = arr[10];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 1 passed: Caught index too large");
        }
        
        // Test 2: Index at boundary
        try {
            int[] arr = new int[5];
            int x = arr[5];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 2 passed: Caught index at boundary");
        }
        
        // Test 3: Array write out of bounds
        try {
            int[] arr = new int[3];
            arr[3] = 42;
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 3 passed: Caught write out of bounds");
        }
        
        // Test 4: Long array out of bounds
        try {
            long[] larr = new long[2];
            long x = larr[5];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 4 passed: Caught long array out of bounds");
        }
        
        // Test 5: Float array out of bounds
        try {
            float[] farr = new float[4];
            float x = farr[10];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            System.out.println("Test 5 passed: Caught float array out of bounds");
        }
        
        // Test 6: Valid access should work
        int[] arr = new int[5];
        arr[0] = 10;
        arr[4] = 20;
        int sum = arr[0] + arr[4];
        System.out.println("Test 6 passed: Valid access works");
        
        System.out.println("excarr.jav worked correctly.");
    }
}

