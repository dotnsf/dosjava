class SwitchDebug2 {
    public static void main() {
        int x;
        
        x = 3;
        System.out.print("Testing x=3: ");
        switch (x) {
            case 1:
                System.out.println("Case 1");
                break;
            case 2:
                System.out.println("Case 2");
                break;
            case 3:
                System.out.println("Case 3");
                break;
            default:
                System.out.println("Default");
                break;
        }
        
        return;
    }
}