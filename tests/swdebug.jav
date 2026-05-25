class SwDebugTest {
    void main() {
        int z = 5;
        int multiResult = 0;
        
        System.out.println("Testing switch with z = " + z);
        
        switch (z) {
            case 1:
                System.out.println("Matched case 1");
                multiResult = 1;
                break;
            case 4:
                System.out.println("Matched case 4");
            case 5:
                System.out.println("Matched case 5");
            case 6:
                System.out.println("Matched case 6");
                multiResult = 2;
                break;
            default:
                System.out.println("Matched default");
                multiResult = 0;
                break;
        }
        
        System.out.println("multiResult = " + multiResult);
    }
}
