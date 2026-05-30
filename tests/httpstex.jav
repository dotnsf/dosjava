// HTTP Status Code Test with Exception Handling
class HttpStatusExTest {
    static void main() {
        int status;
        
        System.println("Testing Http.getStatusCode with exception handling...");
        
        try {
            // Test 1: example.com (should return 200)
            System.println("Test 1: http://example.com");
            status = Http.getStatusCode("http://example.com");
            System.println("Status code: ");
            System.printInt(status);
            System.println("");
            
        } catch (Exception e) {
            System.println("Error occurred: ");
            System.println(e.getMessage());
        }
        
        System.println("Done");
    }
}