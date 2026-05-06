// strext.jav
class strext {
    public static void main() {
        String str = "Hello World!";

        String str1 = str.toUpperCase();
        String str2 = str.toLowerCase();
        System.out.println(str1);  // "HELLO WORLD!"
        System.out.println(str2);  // "hello world!"

        int n1 = str.startsWith( "Hello" );
        int n2 = str.startsWith( "HELLO" );
        int n3 = str.endsWith( "World!" );
        int n4 = str.endsWith( "world!" );
        System.out.println(n1);  // 1
        System.out.println(n2);  // 0
        System.out.println(n3);  // 1
        System.out.println(n4);  // 0

        int n5 = str.indexOf( "W", 3 );
        int n6 = str.indexOf( "!", 3 );
        int n7 = str.indexOf( "o", 3 );
        System.out.println(n5);  // 6
        System.out.println(n6);  // 11
        System.out.println(n7);  // 4

        int n8 = str.lastIndexOf( "W", 3 );
        int n9 = str.lastIndexOf( "!", 3 );
        int n10 = str.lastIndexOf( "o", 3 );
        System.out.println(n8);  // 6
        System.out.println(n9);  // 11
        System.out.println(n10); // 7
    }
}
