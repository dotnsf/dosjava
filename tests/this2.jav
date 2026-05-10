class Counter {
    int count;
    
    void increment() {
        this.count = this.count + 1;
    }
    
    void add(int value) {
        this.count = this.count + value;
    }
    
    int getCount() {
        return this.count;
    }
    
    public static void main() {
        Counter c = new Counter();
        c.increment();
        c.increment();
        c.add(5);
        System.out.println(c.getCount());
    }
}

// Made with Bob
