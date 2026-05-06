// tests/file2.jav
class file2 {
    public static void main() {
        File.open("test1.txt", "w");
        
        File.writeLine( "ABC" );
        File.writeLine( "123" );
        File.writeLine( "DEF" );
        String str = "Hello World!";
        File.writeLine( str );
        
        File.close();

        File.open("test1.txt", "r");
        
        String line1 = File.readLine();
        String line2 = File.readLine();
        
        System.out.println(line1);  // "ABC"
        System.out.println(line2);  // "123"
        
        File.close();
    }
}
