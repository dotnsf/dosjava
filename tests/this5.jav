class Test {
    int x;
    
    void set(int v) {
        x = v;
    }
    
    int get() {
        return x;
    }
    
    public static void main() {
        Test t = new Test();
        t.set(10);
        System.out.println(t.get());
    }
}

// Made with Bob
