class excprint {
    public static void main() {
        try {
            System.out.println("Attempting operation...");
            throw new Exception();
        } catch (Exception e) {
            System.out.println("Error: " + e);
        }
        System.out.println("excprint.jav worked correctly.");
    }
}

// Made with Bob
