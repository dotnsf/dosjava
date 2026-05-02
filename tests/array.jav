// Array test program
// Tests basic array operations: declaration, initialization, access

class Array {
    public static void main() {
        // Test 1: Array declaration and initialization
        int[] arr;
        arr = new int[5];
        
        // Test 2: Array element assignment
        arr[0] = 10;
        arr[1] = 20;
        arr[2] = 30;
        arr[3] = 40;
        arr[4] = 50;
        
        // Test 3: Array element access and print
        // Expected output: 10 20 30 40 50
        System.out.println(arr[0]);
        System.out.println(arr[1]);
        System.out.println(arr[2]);
        System.out.println(arr[3]);
        System.out.println(arr[4]);
        
        // Test 4: Array length
        // Expected output: 5
        int len;
        len = arr.length;
        System.out.println(len);
        
        // Test 5: Array in loop
        // Sum of array elements: 10+20+30+40+50 = 150
        int sum;
        sum = 0;
        int i;
        i = 0;
        while (i < len) {
            sum = sum + arr[i];
            i = i + 1;
        }
        System.out.println(sum);
    }
}


