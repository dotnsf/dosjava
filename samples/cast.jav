class CastTest {
    public static void main() {
        // Test int to long cast
        int i = 100;
        long l1 = (long)i;
        
        // Test long to int cast
        long l2 = 200L;
        int i2 = (int)l2;
        
        // Test int to float cast
        int i3 = 10;
        float f1 = (float)i3;
        
        // Test float to int cast (truncation)
        float f2 = 3.7f;
        int i4 = (int)f2;  // Should be 3
        
        // Test long to float cast
        long l3 = 1000L;
        float f3 = (float)l3;
        
        // Test float to long cast
        float f4 = 99.9f;
        long l4 = (long)f4;  // Should be 99
        
        // Test implicit casts (widening)
        int i5 = 50;
        long l5 = i5;  // Implicit int to long
        float f5 = i5;  // Implicit int to float
        
        // Verify results
        if (l1 == 100L && i2 == 200 && 
            f1 > 9.99f && f1 < 10.01f &&
            i4 == 3 &&
            f3 > 999.9f && f3 < 1000.1f &&
            l4 == 99L &&
            l5 == 50L && f5 > 49.99f && f5 < 50.01f) {
            System.out.println("cast.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
