class Person {
    String name;
    int age;
    
    void setName(String n) {
        name = n;
    }
    
    String getName() {
        return name;
    }
    
    void setAge(int a) {
        age = a;
    }
    
    int getAge() {
        return age;
    }
}

class strfld1 {
    public static void main() {
        Person p = new Person();
        p.setName("Alice");
        p.setAge(25);
        
        System.out.println(p.getName());
        System.out.println(p.getAge());
    }
}
