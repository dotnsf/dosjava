// Test case for error detection
// This file should produce semantic errors

class ErrorTest {
    int field1 = 10;
    
    public void testMethod() {
        // Error: undefined variable
        int x = undefined_var;
        
        // Error: type mismatch (boolean = int)
        boolean b = 5 + 10;
        
        // Error: type mismatch (int = boolean)
        int y = true;
        
        // Error: invalid operand types (int && int)
        boolean result = 5 && 10;
        
        // Error: invalid operand types (boolean + boolean)
        int z = true + false;
        
        // Error: condition must be boolean
        if (42) {
            int a = 1;
        }
        
        // Error: condition must be boolean
        while (100) {
            int b = 2;
        }
        
        // Valid declarations for next errors
        int valid1 = 10;
        int valid2 = 20;
        
        // Error: duplicate variable
        int valid1 = 30;
        
        return;
    }
    
    public int testReturn() {
        // Error: missing return statement
    }
    
    public void testVoidReturn() {
        // Error: return with value in void method
        return 42;
    }
    
    public int testWrongReturn() {
        // Error: wrong return type
        return true;
    }
}


