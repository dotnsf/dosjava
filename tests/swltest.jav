class SwitchLongTest {
    public static void main() {
        long x = 100L;
        System.out.print("x=100L: ");
        
        switch (x) {
            case 100L:
                System.out.println("Match");
                break;
            default:
                System.out.println("No match");
                break;
        }
    }
}