// HTTP Status Code Test
class HttpStatusTest {
    static void main() {
        int status;
        
        System.println("Testing Http.getStatusCode...");
        
        // Test 1: example.com (should return 200)
        System.println("Test 1: http://example.com");
        status = Http.getStatusCode("http://example.com");
        System.println("Status code: ");
        System.printInt(status);
        System.println("");
        
        // Test 2: Invalid URL (should return error code)
        System.println("Test 2: http://invalid.example.test");
        status = Http.getStatusCode("http://invalid.example.test");
        System.println("Status code: ");
        System.printInt(status);
        System.println("");
        
        System.println("Done");
    }
}