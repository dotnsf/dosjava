class DupTest {
    public static void main() {
        int x;
        int y;
        
        x = 1;
        
        // This simulates what switch does:
        // Push x, DUP, compare with 1
        y = x;  // Simulate DUP by copying to another variable
        
        if (y == 1) {
            System.out.println("Match");
        }
        
        return;
    }
}