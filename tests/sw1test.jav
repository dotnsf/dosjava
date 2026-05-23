class Switch1Test {
    public static void main() {
        int x = 1;
        System.out.print("x=");
        System.out.print(x);
        System.out.print(": ");
        
        switch (x) {
            case 1:
                System.out.print("A");
                break;
            case 2:
                System.out.print("B");
                break;
        }
        
        System.out.println("Done");
    }
}