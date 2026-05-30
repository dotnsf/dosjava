// HTTP PUT Test
class HttpPutTest {
    static void main() {
        String url;
        String data;
        String response;
        
        System.println("=== HTTP PUT Test ===");
        System.println("");
        
        // Test: PUT request
        System.println("Test: PUT request to httpbin.org/put");
        url = "http://192.168.0.2:8080/";
        data = "name=updated&value=456";
        
        try {
            response = Http.put(url, data);
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