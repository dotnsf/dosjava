class SwLongTest {
    public static void main() {
        int result = 0;
        
        // Test basic switch with long
        long x = 2000000000L;
        switch (x) {
            case 1000000000L:
                result = 1;
                break;
            case 2000000000L:
                result = 2;
                break;
            case 3000000000L:
                result = 3;
                break;
            default:
                result = 0;
                break;
        }
        
        // Test switch with default
        long y = 9999999999L;
        int defaultResult = 0;
        switch (y) {
            case 1000000000L:
                defaultResult = 1;
                break;
            case 2000000000L:
                defaultResult = 2;
                break;
            default:
                defaultResult = 99;
                break;
        }
        
        // Test switch with multiple cases
        long z = 5000000000L;
        int multiResult = 0;
        switch (z) {
            case 1000000000L:
            case 2000000000L:
            case 3000000000L:
                multiResult = 1;
                break;
            case 4000000000L:
            case 5000000000L:
            case 6000000000L:
                multiResult = 2;
                break;
            default:
                multiResult = 0;
                break;
        }
        
        // Verify results: result=2, defaultResult=99, multiResult=2
        if (result == 2 && defaultResult == 99 && multiResult == 2) {
            System.out.println("swlong.jav worked correctly.");
        }
        
        return;
    }
}
