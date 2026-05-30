// HTTP GET test program
// Tests basic HTTP GET functionality using Http.get()

class HttpGetTest {
    static void main() {
        String url;
        String response;
        
        System.println("=== HTTP GET Test ===");
        System.println("");
        
        // Test 1: Simple GET request
        System.println("Test 1: GET request to example.com");
        url = "http://192.168.0.2:8080";
        
        try {
            response = Http.get(url);
            System.println("Success! Response:");
            System.println(response);
        } catch (Exception e) {
            System.println("Error: ");
            System.println(e.getMessage());
        }
        
        System.println("");
        System.println("=== Test Complete ===");
    }
}