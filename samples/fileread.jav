class FileReadTest {
    public static void main() {
        // Create test file first
        File.open("test.txt", "w");
        File.writeLine("Line 1");
        File.writeLine("Line 2");
        File.close();
        
        // Test file reading
        File.open("test.txt", "r");
        String line1 = File.readLine();
        String line2 = File.readLine();
        File.close();
        
        // Verify results
        if (line1.equals("Line 1") && line2.equals("Line 2")) {
            System.out.println("fileread.jav worked correctly.");
        }
        
        return;
    }
}
