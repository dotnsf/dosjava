// Test all operators
class OperatorTest {
    public static void main() {
        int a = 10;
        int b = 20;
        
        // Arithmetic
        int sum = a + b;
        int diff = a - b;
        int prod = a * b;
        int quot = a / b;
        int rem = a % b;
        
        // Comparison
        boolean eq = (a == b);
        boolean ne = (a != b);
        boolean lt = (a < b);
        boolean le = (a <= b);
        boolean gt = (a > b);
        boolean ge = (a >= b);
        
        // Logical
        boolean and = (a > 5) && (b < 30);
        boolean or = (a < 5) || (b > 30);
        boolean not = !(a == b);
    }
}


