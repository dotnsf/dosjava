// func.jav
class func {
    public static void main() {
        int x1 = func1();
        System.out.println( x1 );
        int x2 = func2( x1 );
        System.out.println( x2 );
        func3( x2 );
        return;
    }
    public static int func1() {
        int r = 1 + 2;
        return r;
    }
    public static int func2( int a ) {
        int r = 2 * a;
        return r;
    }
    public static void func3( int a ) {
        System.out.println( a );
        return;
    }
}
