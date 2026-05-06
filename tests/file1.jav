// tests/file1.jav
class file1 {
    public static void main() {
        File.open("test.txt");
        
        String line1 = File.readLine();
        String line2 = File.readLine();
        String line3 = File.readLine();
        
        System.out.println(line1);
        System.out.println(line2);
        System.out.println(line3);
        
        File.close();
    }
}
