class Student {
    String name;
    int score;
    
    void init(String n, int s) {
        name = n;
        score = s;
    }
    
    String getReport() {
        return name + ": " + score + " points";
    }
}

class strall1 {
    public static void main() {
        Student s1 = new Student();
        s1.init("Alice", 95);
        
        Student s2 = new Student();
        s2.init("Bob", 87);
        
        System.out.println(s1.getReport());
        System.out.println(s2.getReport());
    }
}
