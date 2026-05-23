class mathexp {
    public static void main() {
        float x;
        float y;
        float result;
        
        System.out.println("=== Math.pow() ===");
        x = 2.0f;
        y = 3.0f;
        result = Math.pow(x, y);
        System.out.println(result);
        
        x = 10.0f;
        y = 2.0f;
        result = Math.pow(x, y);
        System.out.println(result);
        
        x = 5.0f;
        y = 0.0f;
        result = Math.pow(x, y);
        System.out.println(result);
        
        System.out.println("=== Math.exp() ===");
        x = 0.0f;
        result = Math.exp(x);
        System.out.println(result);
        
        x = 1.0f;
        result = Math.exp(x);
        System.out.println(result);
        
        x = 2.0f;
        result = Math.exp(x);
        System.out.println(result);
        
        System.out.println("=== Math.log() ===");
        x = 1.0f;
        result = Math.log(x);
        System.out.println(result);
        
        x = 2.71f;
        result = Math.log(x);
        System.out.println(result);
        
        x = 10.0f;
        result = Math.log(x);
        System.out.println(result);
        
        return;
    }
}
