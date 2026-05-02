// Array sorting test - Bubble sort algorithm
// Tests array operations in a practical algorithm

class Arrays {
    public static void main() {
        // Create and initialize array with unsorted values
        int[] arr;
        arr = new int[5];
        arr[0] = 64;
        arr[1] = 34;
        arr[2] = 25;
        arr[3] = 12;
        arr[4] = 22;
        
        // Print original array
        // Expected: 64 34 25 12 22
        int i;
        i = 0;
        while (i < 5) {
            System.out.println(arr[i]);
            i = i + 1;
        }
        
        // Bubble sort
        int n;
        n = 5;
        int j;
        int temp;
        
        i = 0;
        while (i < n - 1) {
            j = 0;
            while (j < n - i - 1) {
                if (arr[j] > arr[j + 1]) {
                    // Swap arr[j] and arr[j+1]
                    temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
                j = j + 1;
            }
            i = i + 1;
        }
        
        // Print sorted array
        // Expected: 12 22 25 34 64
        i = 0;
        while (i < 5) {
            System.out.println(arr[i]);
            i = i + 1;
        }
    }
}


