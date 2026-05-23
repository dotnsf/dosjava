class mathall {
    public static void main() {
        float a;
        float b;
        float c;
        float result;
        float x;
        float y;
        
        System.out.println("=== Pythagorean Theorem Test ===");
        a = 3.0f;
        b = 4.0f;
        
        x = Math.pow(a, 2.0f);
        System.out.println(x);
        
        y = Math.pow(b, 2.0f);
        System.out.println(y);
        
        c = x + y;
        System.out.println(c);
        
        result = Math.sqrt(c);
        System.out.println(result);
        
        System.out.println("=== Circle Area Test ===");
        float radius;
        float area;
        float pi;
        
        pi = 3.14f;
        radius = 5.0f;
        
        area = pi * Math.pow(radius, 2.0f);
        System.out.println(area);
        
        System.out.println("=== Distance Formula Test ===");
        float x1;
        float y1;
        float x2;
        float y2;
        float dist;
        float dx;
        float dy;
        
        x1 = 1.0f;
        y1 = 2.0f;
        x2 = 4.0f;
        y2 = 6.0f;
        
        dx = x2 - x1;
        dy = y2 - y1;
        
        dist = Math.sqrt(Math.pow(dx, 2.0f) + Math.pow(dy, 2.0f));
        System.out.println(dist);
        
        return;
    }
}
