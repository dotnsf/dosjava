// str3.jav
class str3 {
    public static void main() {
        String str = "Hello World!";

        int n1 = str.indexOf( "Hello" );
        int n2 = str.indexOf( "HELLO" );
        int n3 = str.indexOf( "hello" );
        int n4 = str.indexOf( "World!" );
        int n5 = str.indexOf( "WORLD!" );
        int n6 = str.indexOf( "world!" );
        int n7 = str.indexOf( "W", 3 );
        int n8 = str.indexOf( "!", 3 );
        int n9 = str.indexOf( "o", 3 );

        System.out.println(n1);  // 0
        System.out.println(n2);  // -1 
        System.out.println(n3);  // -1

        System.out.println(n4);  // 6
        System.out.println(n5);  // -1
        System.out.println(n6);  // -1

        System.out.println(n7);  // 6
        System.out.println(n8);  // 11
        System.out.println(n9);  // 4
    }
}
