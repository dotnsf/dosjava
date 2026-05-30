// HTTP DELETE Test
class HttpDeleteTest {
    static void main() {
        String url;
        String response;
        
        System.println("=== HTTP DELETE Test ===");
        System.println("");
        
        // Test: DELETE request
        System.println("Test: DELETE request to httpbin.org/delete");
        url = "http://192.168.0.2:8080/";
        
        try {
            response = Http.delete(url);
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