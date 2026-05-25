class SwNestTest {
    public static void main() {
        int result = 0;
        
        // Test nested switch statements
        int x = 2;
        int y = 3;
        
        switch (x) {
            case 1:
                switch (y) {
                    case 1:
                        result = 11;
                        break;
                    case 2:
                        result = 12;
                        break;
                    default:
                        result = 10;
                        break;
                }
                break;
            case 2:
                switch (y) {
                    case 1:
                        result = 21;
                        break;
                    case 2:
                        result = 22;
                        break;
                    case 3:
                        result = 23;
                        break;
                    default:
                        result = 20;
                        break;
                }
                break;
            case 3:
                switch (y) {
                    case 1:
                        result = 31;
                        break;
                    default:
                        result = 30;
                        break;
                }
                break;
            default:
                result = 0;
                break;
        }
        
        // Verify result: x=2, y=3 -> result=23
        if (result == 23) {
            System.out.println("swnest.jav worked correctly.");
        }
        
        return;
    }
}
