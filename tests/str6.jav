// str6.jav
class str6 {
    public static void main() {
        String str = "Hello World!";
        boolean b = str.equals( "Hello World!" );
        if( b ){
            System.out.println( "true" );  //
        }else{
            System.out.println( "false" );
        }

        b = str.equals( "HELLO WORLD!" );
        if( b ){
            System.out.println( "true" );
        }else{
            System.out.println( "false" );  //
        }
    }
}
