// obj3.jav - Test 3: Instance method call
class TestObj3 {
    int value;
    
    public int getValue() {
        return value;
    }
    
    public void setValue(int v) {
        value = v;
    }
    
    public static void main() {
        TestObj3 obj = new TestObj3();
        obj.setValue(42);
        int result = obj.getValue();
        System.out.println(result);
    }
}
