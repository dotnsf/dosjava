class NullTest {
    static void main() {
        String s;
        int[] arr;
        
        System.out.println("Testing null literal");
        
        // Test null assignment
        s = null;
        arr = null;
        
        // Test null comparison
        if (s == null) {
            System.out.println("s is null");
        }
        
        if (arr == null) {
            System.out.println("arr is null");
        }
        
        // Test with non-null values
        s = "hello";
        if (s != null) {
            System.out.println("s is not null");
        }
        
        arr = new int[5];
        if (arr != null) {
            System.out.println("arr is not null");
        }
        
        System.out.println("Done");
    }
}
