class FloatArrTest {
    public static void main() {
        // Test float array declaration and initialization
        float[] arr1 = new float[5];
        arr1[0] = 1.5f;
        arr1[1] = 2.5f;
        arr1[2] = 3.5f;
        arr1[3] = 4.5f;
        arr1[4] = 5.5f;
        
        // Test float array initialization with values
        float[] arr2 = new float[5];
        arr2[0] = 1.0f;
        arr2[1] = 2.0f;
        arr2[2] = 3.0f;
        arr2[3] = 4.0f;
        arr2[4] = 5.0f;
        
        // Test array access and sum
        float sum = 0.0f;
        int i = 0;
        while (i < 5) {
            sum = sum + arr2[i];
            i = i + 1;
        }
        
        // Test array length
        int len = arr1.length;
        
        // Test array modification
        arr1[2] = 9.9f;
        
        // Verify results: sum=15.0, len=5, arr1[2]=9.9
        if (sum > 14.99f && sum < 15.01f && 
            len == 5 && 
            arr1[2] > 9.89f && arr1[2] < 9.91f) {
            System.out.println("floatarr.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
