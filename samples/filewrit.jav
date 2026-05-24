class FileWritTest {
    public static void main() {
        try {
            // Test file writing
            PrintWriter pw = new PrintWriter(new FileWriter("output.txt"));
            pw.println("Test line 1");
            pw.println("Test line 2");
            pw.print("No newline");
            pw.close();
            
            // Read back to verify
            BufferedReader br = new BufferedReader(new FileReader("output.txt"));
            String line1 = br.readLine();
            String line2 = br.readLine();
            String line3 = br.readLine();
            br.close();
            
            // Verify results
            if (line1.equals("Test line 1") && 
                line2.equals("Test line 2") && 
                line3.equals("No newline")) {
                System.out.println("filewrit.jav worked correctly.");
            }
        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
        
        return;
    }
}

// Made with Bob
