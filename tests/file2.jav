// tests/file2.jav
class file2 {
    public static void main() {
        File.open("test1.txt");
        
        File.writeLine( "ABC" );
        File.writeLine( "DEF" );
        File.writeLine( "123" );
        String str = "Hello World!";
        File.writeLine( str );
        
        File.close();
    }
}
