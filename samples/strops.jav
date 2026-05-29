class StrOpsTest {
    public static void main() {
        String s = "Hello World";
        
        // Test length()
        int len = s.length();
        
        // Test substr()
        String sub1 = s.substr(0, 5);    // "Hello"
        String sub2 = s.substr(6);       // "World"
        
        // Test indexOf()
        int idx1 = s.indexOf("o");          // 4
        int idx2 = s.indexOf("W");          // 6
        int idx3 = s.indexOf("x");          // -1
        
        // Test lastIndexOf()
        int lidx = s.lastIndexOf("o");      // 7
        
        // Test toUpperCase()
        String upper = "test".toUpperCase();    // "TEST"
        
        // Test toLowerCase()
        String lower = "TEST".toLowerCase();    // "test"
        
        // Verify results
        if (len == 11 &&
            sub1.equals("Hello") == 1 && sub2.equals("World") == 1 &&
            idx1 == 4 && idx2 == 6 && idx3 == -1 && lidx == 7 &&
            upper.equals("TEST") == 1 && lower.equals("test") == 1) {
            System.out.println("strops.jav worked correctly.");
        }
        
        return;
    }
}
