class Operators {
    public static void main() {
        int a = 10;
        int b = 20;
        int c = a + b * 2;
        int d = (a + b) * 2;
        boolean flag = a < b && c > d;
        
        if (flag) {
            System.out.println(c);
        } else {
            System.out.println(d);
        }
    }
}


