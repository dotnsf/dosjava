class StrOpsTest {
    public static void main() {
        String s = "Hello World";
        
        // Test length()
        int len = s.length();
        
        // Test charAt()
        char c0 = s.charAt(0);      // 'H'
        char c6 = s.charAt(6);      // 'W'
        
        // Test substring()
        String sub1 = s.substring(0, 5);    // "Hello"
        String sub2 = s.substring(6);       // "World"
        
        // Test indexOf()
        int idx1 = s.indexOf('o');          // 4
        int idx2 = s.indexOf('W');          // 6
        int idx3 = s.indexOf('x');          // -1
        
        // Test lastIndexOf()
        int lidx = s.lastIndexOf('o');      // 7
        
        // Test toUpperCase()
        String upper = "test".toUpperCase();    // "TEST"
        
        // Test toLowerCase()
        String lower = "TEST".toLowerCase();    // "test"
        
        // Verify results
        if (len == 11 && c0 == 'H' && c6 == 'W' &&
            sub1.equals("Hello") && sub2.equals("World") &&
            idx1 == 4 && idx2 == 6 && idx3 == -1 && lidx == 7 &&
            upper.equals("TEST") && lower.equals("test")) {
            System.out.println("strops.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
