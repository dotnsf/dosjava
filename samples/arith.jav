class ArithTest {
    public static void main() {
        // Test addition
        int add1 = 5 + 3;
        int add2 = -10 + 15;
        
        // Test subtraction
        int sub1 = 10 - 4;
        int sub2 = 5 - 8;
        
        // Test multiplication
        int mul1 = 6 * 7;
        int mul2 = -3 * 4;
        
        // Test division
        int div1 = 20 / 4;
        int div2 = 17 / 5;
        
        // Test modulo
        int mod1 = 17 % 5;
        int mod2 = 20 % 4;
        
        // Test unary minus
        int neg = -42;
        
        // Test operator precedence
        int prec = 2 + 3 * 4;  // Should be 14, not 20
        
        // Verify results
        if (add1 == 8 && add2 == 5 &&
            sub1 == 6 && sub2 == -3 &&
            mul1 == 42 && mul2 == -12 &&
            div1 == 5 && div2 == 3 &&
            mod1 == 2 && mod2 == 0 &&
            neg == -42 && prec == 14) {
            System.out.println("arith.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
