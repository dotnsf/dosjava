class SwStringTest {
    public static void main() {
        int result = 0;
        
        // Test basic switch with String
        String x = "two";
        switch (x) {
            case "one":
                result = 1;
                break;
            case "two":
                result = 2;
                break;
            case "three":
                result = 3;
                break;
            default:
                result = 0;
                break;
        }
        
        // Test switch with default
        String y = "unknown";
        int defaultResult = 0;
        switch (y) {
            case "one":
                defaultResult = 1;
                break;
            case "two":
                defaultResult = 2;
                break;
            default:
                defaultResult = 99;
                break;
        }
        
        // Test switch with multiple cases
        String z = "hello";
        int multiResult = 0;
        switch (z) {
            case "hi":
            case "hello":
            case "hey":
                multiResult = 1;
                break;
            case "bye":
            case "goodbye":
                multiResult = 2;
                break;
            default:
                multiResult = 0;
                break;
        }
        
        // Verify results: result=2, defaultResult=99, multiResult=1
        if (result == 2 && defaultResult == 99 && multiResult == 1) {
            System.out.println("swstring.jav worked correctly.");
        }
        
        return;
    }
}
