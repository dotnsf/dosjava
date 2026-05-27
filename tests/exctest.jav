class ExcTest {
    public static void main() {
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println(e);
        }
    }
}
