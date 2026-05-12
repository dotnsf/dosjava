// Hex dump utility to see actual byte values
class HexDump {
    static void main() {
        FileInputStream fis;
        int b;
        int count;
        int col;
        
        System.out.println("Hex dump of E2EWRIT.TXT:");
        
        fis = new FileInputStream("E2EWRIT.TXT");
        count = 0;
        col = 0;
        
        b = fis.read();
        while (b != -1) {
            // Print byte value in decimal
            if (b < 10) {
                System.out.print("00");
                System.out.print(b);
            } else if (b < 100) {
                System.out.print("0");
                System.out.print(b);
            } else {
                System.out.print(b);
            }
            System.out.print(" ");
            
            col = col + 1;
            if (col == 16) {
                System.out.println("");
                col = 0;
            }
            
            count = count + 1;
            b = fis.read();
        }
        
        if (col != 0) {
            System.out.println("");
        }
        
        fis.close();
        
        System.out.print("Total bytes: ");
        System.out.println(count);
    }
}

// Made with Bob
