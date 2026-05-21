// tests/exc4.jav
class exc4 {
    public static void main() {
        String line1;
        
        try {
            File.open("test_.txt");
            line1 = File.readLine();
            System.out.println(line1);
            File.close();
        } catch (Exception e) {
            System.out.println("Exception caught");
        }
    }
}
