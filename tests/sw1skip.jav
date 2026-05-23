class Switch1Skip {
    public static void main() {
        int x;
        
        x = 2;
        System.out.print("x=2: ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
        }
        
        x = 1;
        System.out.print("x=1: ");
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
        }
        
        return;
    }
}