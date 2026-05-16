class TryOnly {
    public static void main(String[] args) {
        int x;
        x = 1;
        try {
            x = 2;
        } catch (Exception e) {
            x = 3;
        }
        x = 4;
    }
}

// Made with Bob
