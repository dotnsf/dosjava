class mathbas {
    public static void main() {
        float x;
        float y;
        float result;
        
        System.out.println("=== Math.abs() ===");
        x = -3.5f;
        result = Math.abs(x);
        System.out.println(result);
        
        x = 3.5f;
        result = Math.abs(x);
        System.out.println(result);
        
        System.out.println("=== Math.min() ===");
        x = 3.5f;
        y = 2.1f;
        result = Math.min(x, y);
        System.out.println(result);
        
        x = 1.0f;
        y = 5.0f;
        result = Math.min(x, y);
        System.out.println(result);
        
        System.out.println("=== Math.max() ===");
        x = 3.5f;
        y = 2.1f;
        result = Math.max(x, y);
        System.out.println(result);
        
        x = 1.0f;
        y = 5.0f;
        result = Math.max(x, y);
        System.out.println(result);
        
        System.out.println("=== Math.sqrt() ===");
        x = 9.0f;
        result = Math.sqrt(x);
        System.out.println(result);
        
        x = 16.0f;
        result = Math.sqrt(x);
        System.out.println(result);
        
        x = 2.0f;
        result = Math.sqrt(x);
        System.out.println(result);
        
        return;
    }
}
