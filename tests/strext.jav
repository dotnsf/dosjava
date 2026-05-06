// strext.jav
class strext {
    public static void main() {
        String str = "Hello World!";
        String str1 = str.toUpperCase();
        String str2 = str.toLowerCase();

        System.out.println(str1);  // "HELLO WORLD!"
        System.out.println(str2);  // "hello world!"

        int n1 = str1.startsWith( "Hello" );
        int n2 = str1.startsWith( "HELLO" );
        int n3 = str1.startsWith( "hello" );
        int n4 = str2.startsWith( "Hello" );
        int n5 = str2.startsWith( "HELLO" );
        int n6 = str2.startsWith( "hello" );
        int n7 = str.startsWith( "Hello" );
        int n8 = str.startsWith( "HELLO" );
        int n9 = str.startsWith( "hello" );

        System.out.println(n1);  // 0
        System.out.println(n2);  // 1
        System.out.println(n3);  // 0

        System.out.println(n4);  // 0
        System.out.println(n5);  // 0
        System.out.println(n6);  // 1

        System.out.println(n7);  // 1
        System.out.println(n8);  // 0
        System.out.println(n9);  // 0
    }
}
