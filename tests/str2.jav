// str2.jav
class str2 {
    public static void main() {
        String str = "Hello World!";

        int n1 = str.startsWith( "Hello" );
        int n2 = str.startsWith( "HELLO" );
        int n3 = str.startsWith( "hello" );
        int n4 = str.endsWith( "World!" );
        int n5 = str.endsWith( "WORLD!" );
        int n6 = str.endsWith( "world!" );

        System.out.println(n1);  // 1
        System.out.println(n2);  // 0
        System.out.println(n3);  // 0

        System.out.println(n4);  // 1
        System.out.println(n5);  // 0
        System.out.println(n6);  // 0
    }
}
