class TestLongArray {
    public static void main() {
        System.println("=== Long Array Test ===");
        
        // Test 1: Create long array
        long[] arr = new long[5];
        System.println("Created long array of size 5");
        
        // Test 2: Store long values
        arr[0] = 1000000L;
        arr[1] = 2000000L;
        arr[2] = 3000000L;
        arr[3] = 4000000L;
        arr[4] = 5000000L;
        System.println("Stored 5 long values");
        
        // Test 3: Load and print long values
        System.println("Array contents:");
        int i = 0;
        while (i < 5) {
            long val = arr[i];
            System.print("arr[");
            System.printInt(i);
            System.print("] = ");
            System.printLong(val);
            System.println("");
            i = i + 1;
        }
        
        // Test 4: Calculate sum
        long sum = 0L;
        i = 0;
        while (i < 5) {
            sum = sum + arr[i];
            i = i + 1;
        }
        System.print("Sum = ");
        System.printLong(sum);
        System.println("");
        
        // Test 5: Array length
        int len = arr.length;
        System.print("Array length: ");
        System.printInt(len);
        System.println("");
        
        // Test 6: Modify array element
        arr[2] = 9999999L;
        System.print("Modified arr[2] = ");
        System.printLong(arr[2]);
        System.println("");
        
        // Test 7: Arithmetic with array elements
        long result = arr[0] + arr[1];
        System.print("arr[0] + arr[1] = ");
        System.printLong(result);
        System.println("");
        
        result = arr[4] - arr[3];
        System.print("arr[4] - arr[3] = ");
        System.printLong(result);
        System.println("");
        
        System.println("Long array test complete!");
    }
}
