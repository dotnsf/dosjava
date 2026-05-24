class IfTest {
    public static void main() {
        int x = 10;
        int result = 0;
        
        // Test simple if
        if (x == 10) {
            result = 1;
        }
        
        // Test if-else
        if (x == 5) {
            result = 0;
        } else {
            result = 2;
        }
        
        // Test nested if
        if (x > 5) {
            if (x < 15) {
                result = 3;
            }
        }
        
        // Test else-if chain
        if (x < 5) {
            result = 0;
        } else if (x < 10) {
            result = 0;
        } else if (x == 10) {
            result = 4;
        } else {
            result = 0;
        }
        
        // Verify all tests passed
        if (result == 4) {
            System.out.println("if.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
