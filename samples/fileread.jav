class FileReadTest {
    public static void main() {
        try {
            // Create test file first
            PrintWriter pw = new PrintWriter(new FileWriter("test.txt"));
            pw.println("Line 1");
            pw.println("Line 2");
            pw.close();
            
            // Test file reading
            BufferedReader br = new BufferedReader(new FileReader("test.txt"));
            String line1 = br.readLine();
            String line2 = br.readLine();
            String line3 = br.readLine();  // Should be null
            br.close();
            
            // Verify results
            if (line1.equals("Line 1") && line2.equals("Line 2") && line3 == null) {
                System.out.println("fileread.jav worked correctly.");
            }
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
        
        return;
    }
}

// Made with Bob
