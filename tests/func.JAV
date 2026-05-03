// func.jav
class func {
    public static void main() {
        int x1 = func1();
        System.out.println( x1 );
        int x2 = func1( x1 );
        System.out.println( x2 );
        return;
    }
    public static void func1() {
        int r = 1 + 2;
        return r;
    }
    public static void func2( int a ) {
        int r = 2 * a;
        return r;
    }
}
