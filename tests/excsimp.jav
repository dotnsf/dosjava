class ExcSimple {
    public static void main() {
        try {
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Caught exception");
        }
    }
}

// Made with Bob
