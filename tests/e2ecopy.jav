// E2E Test: File copy using FileInputStream and FileOutputStream
class E2ECopy {
    static void main() {
        FileInputStream fis;
        FileOutputStream fos;
        int b;
        int count;
        
        System.out.println("Copying E2EWRIT.TXT to E2ECOPY.TXT");
        
        fis = new FileInputStream("E2EWRIT.TXT");
        fos = new FileOutputStream("E2ECOPY.TXT");
        
        count = 0;
        b = fis.read();
        while (b != -1) {
            //System.out.println( b );
            fos.write(b);
            count = count + 1;
            b = fis.read();
        }
        
        fis.close();
        fos.close();
        
        System.out.println("Copy test completed");
        System.out.print("Copied ");
        System.out.print(count);
        System.out.println(" bytes");
    }
}

// Made with Bob
