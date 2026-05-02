// Test case for scope resolution
// This file should pass semantic analysis without errors

class ScopeTest {
    int field1 = 5;
    int field2 = 10;
    
    public void method1(int param1, int param2) {
        // Parameters are accessible
        int local1 = param1 + param2;
        
        // Fields are accessible
        int local2 = field1 + field2;
        
        // Nested scope
        if (local1 > 10) {
            int inner1 = local1 * 2;
            int inner2 = inner1 + local2;
            
            // Outer variables accessible
            local1 = inner2;
        }
        
        // Another nested scope
        while (local2 < 100) {
            int inner3 = local2 + 5;
            local2 = inner3;
        }
        
        // Block scope
        {
            int block1 = 42;
            int block2 = block1 + local1;
        }
        
        return;
    }
    
    public void method2() {
        // Different method scope
        int local1 = 100;
        int local2 = 200;
        
        // Fields still accessible
        int result = field1 + field2 + local1 + local2;
        
        return;
    }
}


