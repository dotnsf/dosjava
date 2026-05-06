// tests/logic.jav
// Test logical operators: &&, ||, !
class logic {
    public static void main() {
        int x = 10;
        int y = 20;
        int result = 0;
        
        // Test AND
        if (x > 5 && y > 15) {
            result = result + 10;  // Should execute
        }
        
        // Test OR
        if (x < 5 || y > 15) {
            result = result + 20;  // Should execute (second condition is true)
        }
        
        // Test NOT
        if (!(x > 100)) {
            result = result + 30;  // Should execute (x is not > 100)
        }
        
        // Test combined
        if ((x > 5 && y < 30) || x > 100) {
            result = result + 40;  // Should execute (first part is true)
        }
        
        System.out.println(result);  // Expected: 100 (10+20+30+40)
    }
}

