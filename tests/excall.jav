class ExcAll {
    public static void main(String[] args) {
        System.out.println("Exception Test");
        
        int pass = 0;
        int total = 10;
        
        // Test 1: NullPointerException
        System.out.println("T1: NullPtr");
        try {
            int[] arr = null;
            int x = arr[0];
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 1) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 2: NullPointerException
        System.out.println("T2: NullPtr");
        try {
            int[] arr = null;
            int len = arr.length;
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 1) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 3: ArrayIndexOutOfBoundsException
        System.out.println("T3: ArrIdx");
        try {
            int[] arr = new int[5];
            int x = arr[-1];
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 2) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 4: ArrayIndexOutOfBoundsException
        System.out.println("T4: ArrIdx");
        try {
            int[] arr = new int[5];
            int x = arr[10];
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 2) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 5: NumberFormatException
        System.out.println("T5: NumFmt");
        try {
            int num = Integer.parseInt("abc");
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 3) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 6: NumberFormatException
        System.out.println("T6: NumFmt");
        try {
            int num = Integer.parseInt("");
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 3) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 7: IllegalArgumentException
        System.out.println("T7: IllArg");
        try {
            int[] arr = new int[-5];
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 4) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 8: IllegalArgumentException
        System.out.println("T8: IllArg");
        try {
            String s = "hello";
            String sub = s.substr(3, 1);
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 4) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 9: StringIndexOutOfBoundsException
        System.out.println("T9: StrIdx");
        try {
            String s = "hello";
            String sub = s.substr(10);
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 5) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Test 10: StringIndexOutOfBoundsException
        System.out.println("T10: StrIdx");
        try {
            String s = "hello";
            String sub = s.substr(0, 10);
            System.out.println("FAIL");
        } catch (Exception e) {
            if (e.getType() == 5) {
                System.out.println("PASS");
                pass = pass + 1;
            } else {
                System.out.println("FAIL");
            }
        }
        
        // Summary
        System.out.println("Result:");
        System.out.println(pass);
        System.out.println(total);
    }
}
