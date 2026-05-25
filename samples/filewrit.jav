class FileWritTest {
    public static void main() {
        // Test file writing
        File.open("output.txt", "w");
        File.writeLine("Test line 1");
        File.writeLine("Test line 2");
        File.writeLine("No newline");
        File.close();
        
        // Read back to verify
        File.open("output.txt", "r");
        String line1 = File.readLine();
        String line2 = File.readLine();
        String line3 = File.readLine();
        File.close();
        
        // Verify results
        if (line1.equals("Test line 1") && 
            line2.equals("Test line 2") && 
            line3.equals("No newline")) {
            System.out.println("filewrit.jav worked correctly.");
        }
        
        return;
    }
}
