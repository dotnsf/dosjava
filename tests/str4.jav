// str4.jav
class str4 {
    public static void main() {
        String str = "Hello World!";

        int n1 = str.lastIndexOf( "Hello" );
        int n2 = str.lastIndexOf( "HELLO" );
        int n3 = str.lastIndexOf( "hello" );
        int n4 = str.lastIndexOf( "World!" );
        int n5 = str.lastIndexOf( "WORLD!" );
        int n6 = str.lastIndexOf( "world!" );
        int n7 = str.lastIndexOf( "W", 3 );
        int n8 = str.lastIndexOf( "!", 3 );
        int n9 = str.lastIndexOf( "o", 3 );

        System.out.println(n1);  // 0
        System.out.println(n2);  // -1 
        System.out.println(n3);  // -1

        System.out.println(n4);  // 6
        System.out.println(n5);  // -1
        System.out.println(n6);  // -1

        System.out.println(n7);  // 6
        System.out.println(n8);  // 11
        System.out.println(n9);  // 7
    }
}
