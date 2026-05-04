// Calculation Test
class Calc {
    public static void main() {
        int x = 10;
        int y = 2;
        int z = 0;
        
        // Add
        z = x + y;
        System.out.println(z); // 12
        
        // Sub
        z = x - y;
        System.out.println(z); // 8
        
        // Mul
        z = x * y;
        System.out.println(z); // 20
        
        // Div
        z = x / y;
        System.out.println(z); // 5
        
        // Mod
        z = x % y;
        System.out.println(z); // 0
        
        // Inc
        z ++;
        System.out.println(z); // 1
        
        // Dec
        z --;
        System.out.println(z); // 0

        // +=
        z += 2;
        System.out.println(z); // 2

        // -=
        z -= x;
        System.out.println(z); // -8

        return;
    }
}


