class ExceptionTest2 {
    public static void main() {
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Caught: " + e);
        }
    }
}

// Made with Bob
