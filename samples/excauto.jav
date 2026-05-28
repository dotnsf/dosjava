class ExcAuto {
    public static void main() {
        int pass = 0;
        int total = 10;
        
        // Test 1: NullPointerException
        try {
            int[] arr = null;
            int x = arr[0];
        } catch (Exception e) {
            if (e.getType() == 1) {
                pass = pass + 1;
            }
        }
        
        // Test 2: NullPointerException
        try {
            int[] arr = null;
            int len = arr.length;
        } catch (Exception e) {
            if (e.getType() == 1) {
                pass = pass + 1;
            }
        }
        
        // Test 3: ArrayIndexOutOfBoundsException
        try {
            int[] arr = new int[5];
            int x = arr[-1];
        } catch (Exception e) {
            if (e.getType() == 2) {
                pass = pass + 1;
            }
        }
        
        // Test 4: ArrayIndexOutOfBoundsException
        try {
            int[] arr = new int[5];
            int x = arr[10];
        } catch (Exception e) {
            if (e.getType() == 2) {
                pass = pass + 1;
            }
        }
        
        // Test 5: NumberFormatException
        try {
            int num = Integer.parseInt("abc");
        } catch (Exception e) {
            if (e.getType() == 3) {
                pass = pass + 1;
            }
        }
        
        // Test 6: NumberFormatException
        try {
            int num = Integer.parseInt("");
        } catch (Exception e) {
            if (e.getType() == 3) {
                pass = pass + 1;
            }
        }
        
        // Test 7: IllegalArgumentException
        try {
            int[] arr = new int[-5];
        } catch (Exception e) {
            if (e.getType() == 4) {
                pass = pass + 1;
            }
        }
        
        // Test 8: IllegalArgumentException
        try {
            String s = "hello";
            String sub = s.substr(3, 1);
        } catch (Exception e) {
            if (e.getType() == 4) {
                pass = pass + 1;
            }
        }
        
        // Test 9: StringIndexOutOfBoundsException
        try {
            String s = "hello";
            String sub = s.substr(10);
        } catch (Exception e) {
            if (e.getType() == 5) {
                pass = pass + 1;
            }
        }
        
        // Test 10: StringIndexOutOfBoundsException
        try {
            String s = "hello";
            String sub = s.substr(0, 10);
        } catch (Exception e) {
            if (e.getType() == 5) {
                pass = pass + 1;
            }
        }
        
        // Verify all tests passed
        if (pass == total) {
            System.out.println("excauto.jav worked correctly.");
        }
        
        return;
    }
}
