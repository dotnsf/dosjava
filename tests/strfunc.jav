// strfunc.jav
class strfunc {
    public static void main() {
        String a = "ABC";

        func1( a );                // "ABC"

        String x2 = func2( "Hello ", a );
        System.out.println( x2 );  // "Hello ABC"

        String x3 = func3( a, 3 );
        System.out.println( x3 );  // "ABCABCABC"

    }
    public static void func1( String str ) {
        System.out.println( str );
        return;
    }
    public static String func2( String str1, String str2 ) {
        return ( str1 + str2 );
    }
    public static String func3( String str, int n ) {
        String r = "";
        for ( int i = 0; i < n; i++ ) {
            r += str;
        }
    
        return r;
    }
}
