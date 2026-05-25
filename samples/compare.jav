class CompareTest {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = 10;
        
        // Test equality
        boolean eq1 = (a == c);     // true
        boolean eq2 = (a == b);     // false
        
        // Test inequality
        boolean ne1 = (a != b);     // true
        boolean ne2 = (a != c);     // false
        
        // Test less than
        boolean lt1 = (a < b);      // true
        boolean lt2 = (b < a);      // false
        boolean lt3 = (a < c);      // false
        
        // Test greater than
        boolean gt1 = (b > a);      // true
        boolean gt2 = (a > b);      // false
        boolean gt3 = (a > c);      // false
        
        // Test less than or equal
        boolean le1 = (a <= b);     // true
        boolean le2 = (a <= c);     // true
        boolean le3 = (b <= a);     // false
        
        // Test greater than or equal
        boolean ge1 = (b >= a);     // true
        boolean ge2 = (a >= c);     // true
        boolean ge3 = (a >= b);     // false
        
        // Verify all results
        if (eq1 && !eq2 && ne1 && !ne2 &&
            lt1 && !lt2 && !lt3 &&
            gt1 && !gt2 && !gt3 &&
            le1 && le2 && !le3 &&
            ge1 && ge2 && !ge3) {
            System.out.println("compare.jav worked correctly.");
        }
        
        return;
    }
}
