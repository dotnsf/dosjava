class WhileTrueTest {
    public static void main() {
        int count = 0;
        while (true) {
            count = count + 1;
            if (count == 3) {
                break;
            }
        }
        System.out.println("Count reached 3");
        return;
    }
}
