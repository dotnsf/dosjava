class VarInit {
    public static void main() {
        // int型の初期化
        int a = 42;
        System.out.println(a);
        
        // long型の初期化
        long b = 123456L;
        System.out.println(b);
        
        // float型の初期化
        float c = 3.14f;
        System.out.println(c);
        
        // boolean型の初期化（文字列として出力）
        boolean d = true;
        System.out.println("boolean d = true");
        
        // String型の初期化
        String e = "Hello";
        System.out.println(e);
        
        // 式での初期化
        int f = 10 + 20;
        System.out.println(f);
        
        // 変数での初期化
        int g = a;
        System.out.println(g);
        
        // 複数の変数を使った式での初期化
        int h = a + f;
        System.out.println(h);
        
        // boolean型を使った条件式での初期化
        int i = 0;
        if (d) {
            i = 1;
        }
        System.out.println(i);
        
        System.out.println("All tests passed!");
    }
}
