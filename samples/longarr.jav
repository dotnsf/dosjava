class LongArrTest {
    public static void main() {
        // Test long array declaration and initialization
        long[] arr1 = new long[5];
        arr1[0] = 1000000000L;
        arr1[1] = 2000000000L;
        arr1[2] = 3000000000L;
        arr1[3] = 4000000000L;
        arr1[4] = 5000000000L;
        
        // Test long array initialization with values
        long[] arr2 = {100L, 200L, 300L, 400L, 500L};
        
        // Test array access and sum
        long sum = 0L;
        int i = 0;
        while (i < 5) {
            sum = sum + arr2[i];
            i = i + 1;
        }
        
        // Test array length
        int len = arr1.length;
        
        // Test array modification
        arr1[2] = 9999999999L;
        
        // Verify results: sum=1500, len=5, arr1[2]=9999999999
        if (sum == 1500L && len == 5 && arr1[2] == 9999999999L) {
            System.out.println("longarr.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
