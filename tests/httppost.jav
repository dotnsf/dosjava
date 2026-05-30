// HTTP POST Test
class HttpPostTest {
    static void main() {
        String url;
        String data;
        String response;
        
        System.println("=== HTTP POST Test ===");
        System.println("");
        
        // Test 1: Simple POST request
        System.println("Test 1: POST request to httpbin.org/post");
        url = "http://192.168.0.2:8080/";
        data = "name=test&value=123";
        
        try {
            response = Http.post(url, data);
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