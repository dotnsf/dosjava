// tests/files.jav
class files {
    public static void main() {
        File.open("test0.txt", "w");
        File.writeLine( "ABC" );
        File.close();

        File.open("test0.txt", "a");
        File.writeLine( "123" );
        File.close();

        File.open("test0.txt", "a");
        String str = "Hello World!";
        File.writeLine( str );
        File.close();

        File.open("test0.txt", "r");

        String line1 = File.readLine();
        String line2 = File.readLine();
        String line3 = File.readLine();
        
        System.out.println(line1);  // "ABC"
        System.out.println(line2);  // "123"
        System.out.println(line3);  // "Hello World!"
        
        File.close();
    }
}
