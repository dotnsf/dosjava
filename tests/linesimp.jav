class LineSimple {
    public static void main() {
        System.out.println("Test start");
        try {
            int a = 10;
            int b = 0;
            int c = a / b;
        } catch (Exception e) {
            System.out.println(e);
        }
        System.out.println("Test end");
    }
}
