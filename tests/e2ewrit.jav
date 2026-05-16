// E2E Test: BufferedWriter with newLine()
class E2EWrite {
    static void main() {
        FileOutputStream fos;
        BufferedWriter bw;
        
        fos = new FileOutputStream("E2EWRIT.TXT");
        bw = new BufferedWriter(fos);
        
        bw.write("Line 1");
        bw.newLine();
        bw.write("Line 2");
        bw.newLine();
        bw.write("Line 3");
        bw.newLine();
        
        bw.close();
        
        System.out.println("Write test completed");
    }
}
