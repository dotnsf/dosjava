class Test {
    int value;
    
    void setValue(int v) {
        this.value = v;
    }
    
    int getValue() {
        return this.value;
    }
    
    public static void main() {
        Test t = new Test();
        System.out.println(t.getValue());
        t.setValue(42);
        System.out.println(t.getValue());
    }
}
