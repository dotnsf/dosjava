class BoolTest {
    public static void main() {
        // Test boolean variable declaration and assignment
        boolean t = true;
        boolean f = false;
        
        // Test boolean operations
        boolean and1 = t && t;  // true
        boolean and2 = t && f;  // false
        boolean or1 = t || f;   // true
        boolean or2 = f || f;   // false
        boolean not1 = !t;      // false
        boolean not2 = !f;      // true

        // Verify results
        if (and1 && !and2 && or1 && !or2 && !not1 && not2) {
            System.out.println("bool.jav worked correctly.");
        }
        
        return;
    }
}
