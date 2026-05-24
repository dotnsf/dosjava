class SwIntTest {
    public static void main() {
        int result = 0;
        
        // Test basic switch with int
        int x = 2;
        switch (x) {
            case 1:
                result = 1;
                break;
            case 2:
                result = 2;
                break;
            case 3:
                result = 3;
                break;
            default:
                result = 0;
                break;
        }
        
        // Test switch with default
        int y = 99;
        int defaultResult = 0;
        switch (y) {
            case 1:
                defaultResult = 1;
                break;
            case 2:
                defaultResult = 2;
                break;
            default:
                defaultResult = 99;
                break;
        }
        
        // Test switch with multiple cases
        int z = 5;
        int multiResult = 0;
        switch (z) {
            case 1:
            case 2:
            case 3:
                multiResult = 1;
                break;
            case 4:
            case 5:
            case 6:
                multiResult = 2;
                break;
            default:
                multiResult = 0;
                break;
        }
        
        // Verify results: result=2, defaultResult=99, multiResult=2
        if (result == 2 && defaultResult == 99 && multiResult == 2) {
            System.out.println("swint.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
