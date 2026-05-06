/*
 * Test: Method with Return Value
 * 
 * Tests method invocation with return value.
 * Expected output: Program should execute without errors.
 */

class TestMethodReturn {
    public static void main() {
        int result = getValue();
        return;
    }
    
    public static int getValue() {
        int value = 42;
        return value;
    }
}


