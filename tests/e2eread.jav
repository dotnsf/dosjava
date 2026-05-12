// E2E Test: BufferedReader with readLine()
class E2ERead {
    static void main() {
        FileInputStream fis;
        BufferedReader br;
        String line;
        
        fis = new FileInputStream("E2EWRIT.TXT");
        br = new BufferedReader(fis);
        
        System.out.println("Reading file:");
        
        line = br.readLine();
        while (line != 0) {
            System.out.println(line);
            line = br.readLine();
        }
        
        br.close();
        
        System.out.println("Read test completed");
    }
}

// Made with Bob
