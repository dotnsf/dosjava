class ftst2 {
    public static void main() {
        float x;
        float y;
        
        x = 1.5f;
        System.out.println(x);
        
        y = 10.0f;
        x = y;
        System.out.println(x);
        
        x = 3.7f;
        y = x / 1.0f;
        System.out.println(y);
        
        x = 2.5f;
        if (x > 2.0f) {
            System.out.println(1);
        }
        
        return;
    }
}
