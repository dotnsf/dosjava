class ForTest {
    public static void main() {
        // Test basic for loop (sum 1 to 10)
        int sum = 0;
        int i;
        for (i = 1; i <= 10; i = i + 1) {
            sum = sum + i;
        }
        
        // Test for loop with break
        int count = 0;
        int j;
        for (j = 0; j < 100; j = j + 1) {
            count = count + 1;
            if (count == 5) {
                break;
            }
        }
        
        // Test for loop with continue
        int evenSum = 0;
        int k;
        for (k = 1; k <= 10; k = k + 1) {
            if (k % 2 == 1) {
                continue;
            }
            evenSum = evenSum + k;
        }
        
        // Test nested for loops
        int product = 0;
        int m;
        int n;
        for (m = 1; m <= 3; m = m + 1) {
            for (n = 1; n <= 3; n = n + 1) {
                product = product + (m * n);
            }
        }
        
        // Verify results: sum=55, count=5, evenSum=30, product=36
        if (sum == 55 && count == 5 && evenSum == 30 && product == 36) {
            System.out.println("for.jav worked correctly.");
        }
        
        return;
    }
}
