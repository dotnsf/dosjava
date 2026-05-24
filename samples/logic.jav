class LogicTest {
    public static void main() {
        boolean t = true;
        boolean f = false;
        
        // Test AND operator
        boolean and1 = t && t;      // true
        boolean and2 = t && f;      // false
        boolean and3 = f && t;      // false
        boolean and4 = f && f;      // false
        
        // Test OR operator
        boolean or1 = t || t;       // true
        boolean or2 = t || f;       // true
        boolean or3 = f || t;       // true
        boolean or4 = f || f;       // false
        
        // Test NOT operator
        boolean not1 = !t;          // false
        boolean not2 = !f;          // true
        
        // Test compound expressions
        boolean comp1 = (t && t) || f;          // true
        boolean comp2 = t && (t || f);          // true
        boolean comp3 = !(t && f);              // true
        boolean comp4 = (!t) || (!f);           // true
        
        // Test short-circuit evaluation (should not cause errors)
        boolean sc1 = f && t;       // false (second operand not evaluated)
        boolean sc2 = t || f;       // true (second operand not evaluated)
        
        // Verify all results
        if (and1 && !and2 && !and3 && !and4 &&
            or1 && or2 && or3 && !or4 &&
            !not1 && not2 &&
            comp1 && comp2 && comp3 && comp4 &&
            !sc1 && sc2) {
            System.out.println("logic.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
