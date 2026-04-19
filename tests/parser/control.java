class Control {
    public static void main() {
        int i = 0;
        int sum = 0;
        
        while (i < 10) {
            sum = sum + i;
            i = i + 1;
        }
        
        if (sum > 40) {
            System.out.println(sum);
            return;
        }
        
        System.out.println(0);
    }
}

// Made with Bob
