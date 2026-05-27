// Stack Overflow Test
// This program intentionally causes a stack overflow
// by calling a recursive function without a base case

class StackOverflowTest {
    static int counter;
    
    public static void main() {
        counter = 0;
        System.out.println("Starting infinite recursion...");
        infiniteRecursion();
        System.out.println("This line should never be reached");
    }
    
    public static void infiniteRecursion() {
        counter = counter + 1;
        
        // Print progress every 100 calls
        if (counter % 100 == 0) {
            System.out.print("Recursion depth: ");
            System.out.println(counter);
        }
        
        // Recursive call without base case - will cause stack overflow
        infiniteRecursion();
    }
}
