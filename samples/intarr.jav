class IntArrTest {
    public static void main() {
        // Test array declaration and initialization
        int[] arr1 = new int[5];
        arr1[0] = 10;
        arr1[1] = 20;
        arr1[2] = 30;
        arr1[3] = 40;
        arr1[4] = 50;
        
        // Test array initialization with values
        int[] arr2 = new int[5];
        arr2[0] = 1;
        arr2[1] = 2;
        arr2[2] = 3;
        arr2[3] = 4;
        arr2[4] = 5;
        
        // Test array access
        int sum = 0;
        int i = 0;
        while (i < 5) {
            sum = sum + arr1[i];
            i = i + 1;
        }
        
        // Test array length
        int len = arr2.length;
        
        // Test array modification
        arr2[2] = 10;
        
        // Verify results: sum=150, len=5, arr2[2]=10
        if (sum == 150 && len == 5 && arr2[2] == 10) {
            System.out.println("intarr.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
