class BoolArrTest {
    public static void main() {
        // Test boolean array declaration and initialization
        boolean[] arr1 = new boolean[5];
        arr1[0] = true;
        arr1[1] = false;
        arr1[2] = true;
        arr1[3] = false;
        arr1[4] = true;
        
        // Test boolean array initialization with values
        boolean[] arr2 = new boolean[4];
        arr2[0] = true;
        arr2[1] = true;
        arr2[2] = false;
        arr2[3] = false;
        
        
        // Test array access and counting
        int trueCount = 0;
        int i = 0;
        while (i < 5) {
            if (arr1[i]) {
                trueCount = trueCount + 1;
            }
            i = i + 1;
        }
        
        // Test array length
        int len = arr2.length;
        
        // Test array modification
        arr2[2] = true;
        
        // Verify results: trueCount=3, len=4, arr2[2]=true
        if (trueCount == 3 && len == 4 && arr2[2]) {
            System.out.println("boolarr.jav worked correctly.");
        }
        
        return;
    }
}
