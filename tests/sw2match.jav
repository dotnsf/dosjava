class Switch2Match {
    public static void main() {
        int x = 2;
        System.out.print("x=");
        System.out.print(x);
        System.out.print(": ");
        
        switch (x) {
            case 1:
                System.out.println("One");
                break;
            case 2:
                System.out.println("Two");
                break;
        }
        
        System.out.println("Done");
    }
}