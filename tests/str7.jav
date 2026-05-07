// str7.jav
class str7 {
    public static void main() {
        String str = "Hello World!";

        int n = str.compareTo( "Hello World!" );
        System.out.println( n );  // 0

        n = str.compareTo( "HELLO WORLD" );
        System.out.println( n );  // plus
        n = str.compareTo( "Hello world!" );
        System.out.println( n );  // minus
        n = str.compareTo( "Hello" );
        System.out.println( n );  // plus
    }
}
