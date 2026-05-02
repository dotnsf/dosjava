// Test case for type checking
// This file should pass semantic analysis without errors

class TypeTest {
    public static void main() {
        // Integer operations
        int a = 10;
        int b = 20;
        int c = a + b * 2;
        int d = (a + b) * 2;
        int e = a - b;
        int f = a / b;
        int g = a % b;
        
        // Boolean operations
        boolean flag1 = true;
        boolean flag2 = false;
        boolean flag3 = flag1 && flag2;
        boolean flag4 = flag1 || flag2;
        boolean flag5 = !flag1;
        
        // Relational operations
        boolean cmp1 = a < b;
        boolean cmp2 = a <= b;
        boolean cmp3 = a > b;
        boolean cmp4 = a >= b;
        boolean cmp5 = a == b;
        boolean cmp6 = a != b;
        
        // Complex expressions
        boolean result = (a + b) > (c - d) && flag1;
        
        // Control flow with correct types
        if (result) {
            int x = 100;
        }
        
        while (a < 100) {
            a = a + 1;
        }
        
        // Return statement
        return;
    }
}


