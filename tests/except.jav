class ExceptionTest {
    public static void main(String[] args) {
        try {
            System.out.println("In try block");
            throw 1;
            System.out.println("In try block 2");
        } catch (Exception e) {
            System.out.println("Caught exception");
        } finally {
            System.out.println("In finally block");
        }
    }
}

// Made with Bob
