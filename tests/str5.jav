// str5.jav
class str5 {
    public static void main() {
        String str = "Hello World!";
        String str1 = str.substr( 1, 3 );
        String str2 = str.substr( 6 );

        System.out.println(str1);  // "el"
        System.out.println(str2);  // "World!"
    }
}
