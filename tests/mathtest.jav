class mathtest {
    public static void main() {
        float a;
        float b;
        float result;
        
        System.out.println("=== Simple multiplication ===");
        a = 3.14f;
        b = 25.0f;
        result = a * b;
        System.out.println(result);
        
        System.out.println("=== Multiplication with pow ===");
        a = 3.14f;
        b = Math.pow(5.0f, 2.0f);
        System.out.println(b);
        result = a * b;
        System.out.println(result);
        
        System.out.println("=== Direct multiplication with pow ===");
        result = 3.14f * Math.pow(5.0f, 2.0f);
        System.out.println(result);
        
        return;
    }
}
