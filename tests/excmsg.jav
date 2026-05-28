class ExcMsg {
    public static void main(String[] args) {
        System.out.println("Testing Exception.getType() and Exception.getMessage()...");
        
        // Test 1: NullPointerException
        try {
            int[] arr = null;
            int x = arr[0];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            int type = e.getType();
            String msg = e.getMessage();
            System.out.println("Test 1: type=" + type + ", msg=" + msg);
        }
        
        // Test 2: ArrayIndexOutOfBoundsException
        try {
            int[] arr = new int[5];
            int x = arr[10];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            int type = e.getType();
            String msg = e.getMessage();
            System.out.println("Test 2: type=" + type + ", msg=" + msg);
        }
        
        // Test 3: NumberFormatException
        try {
            int num = Integer.parseInt("abc");
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            int type = e.getType();
            String msg = e.getMessage();
            System.out.println("Test 3: type=" + type + ", msg=" + msg);
        }
        
        // Test 4: IllegalArgumentException
        try {
            int[] arr = new int[-5];
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            int type = e.getType();
            String msg = e.getMessage();
            System.out.println("Test 4: type=" + type + ", msg=" + msg);
        }
        
        // Test 5: StringIndexOutOfBoundsException
        try {
            String s = "hello";
            String sub = s.substr(10);
            System.out.println("ERROR: Should have thrown exception");
        } catch (Exception e) {
            int type = e.getType();
            String msg = e.getMessage();
            System.out.println("Test 5: type=" + type + ", msg=" + msg);
        }
        
        System.out.println("All exception message retrieval tests completed");
    }
}
