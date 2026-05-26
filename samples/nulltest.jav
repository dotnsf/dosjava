class NullTest {
    static void main() {
        String s1;
        String s2;
        int[] arr1;
        int[] arr2;
        boolean ok;
        
        // Test null assignment to String
        s1 = null;
        ok = true;
        
        // Test null assignment to array
        arr1 = null;
        
        // Test null comparison with ==
        if (s1 == null) {
            ok = ok && true;
        } else {
            ok = false;
        }
        
        if (arr1 == null) {
            ok = ok && true;
        } else {
            ok = false;
        }
        
        // Test null comparison with !=
        s2 = "hello";
        if (s2 != null) {
            ok = ok && true;
        } else {
            ok = false;
        }
        
        arr2 = new int[5];
        if (arr2 != null) {
            ok = ok && true;
        } else {
            ok = false;
        }
        
        // Test comparing two null values
        s1 = null;
        s2 = null;
        if (s1 == s2) {
            ok = ok && true;
        } else {
            ok = false;
        }
        
        // Test comparing null with non-null
        s1 = null;
        s2 = "world";
        if (s1 != s2) {
            ok = ok && true;
        } else {
            ok = false;
        }
        
        if (ok) {
            System.out.println("nulltest.jav worked correctly.");
        }
    }
}
