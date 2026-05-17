class Test0 {
    public static void main(String[] args) {
        try {
            System.out.println("1");
            int x = 5 / 0;
            System.out.println("2");
        } catch (Exception e) {
            System.out.println("3");
        }
    }
}
