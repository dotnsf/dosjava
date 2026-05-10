class Test {
    int x;
    
    void set(int v) {
        this.x = v;
    }
    
    int get() {
        return this.x;
    }
    
    public static void main() {
        Test t = new Test();
        t.set(10);
        System.out.println(t.get());
    }
}

// Made with Bob
