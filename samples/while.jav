class WhileTest {
    public static void main() {
        int sum = 0;
        int i = 1;
        
        // Test basic while loop (sum 1 to 10)
        while (i <= 10) {
            sum = sum + i;
            i = i + 1;
        }
        
        // Test while with break
        int count = 0;
        while (true) {
            count = count + 1;
            if (count == 5) {
                break;
            }
        }
        
        // Test while with continue
        int evenSum = 0;
        int j = 0;
        while (j < 10) {
            j = j + 1;
            if (j % 2 == 1) {
                continue;
            }
            evenSum = evenSum + j;
        }
        
        // Verify results: sum=55, count=5, evenSum=30
        if (sum == 55 && count == 5 && evenSum == 30) {
            System.out.println("while.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
