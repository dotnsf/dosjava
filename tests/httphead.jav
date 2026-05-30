class HttpHeaderTest {
    static void main() {
        System.println("Testing HTTP GET with custom headers...");
        System.println("");
        
        try {
            // Test 1: GET with custom headers (newline-separated)
            System.println("Test 1: GET with custom headers");
            String headers = "Content-Type: application/json\nUser-Agent: DOSJava/1.0\nAccept: */*";
            String response = Http.get("http://example.com/", headers);
            System.println("Response with headers:");
            System.println(response);
            System.println("");
            
        } catch (Exception e) {
            System.println("Test 1 failed!");
            System.println("Type: ");
            System.printInt(e.getType());
            System.println("");
            System.println("Message: ");
            System.println(e.getMessage());
            System.println("");
        }
        
        try {
            // Test 2: GET without headers (original method)
            System.println("Test 2: GET without headers");
            String response2 = Http.get("http://example.com/");
            System.println("Response without headers:");
            System.println(response2);
            System.println("");
            
        } catch (Exception e) {
            System.println("Test 2 failed!");
            System.println("Type: ");
            System.printInt(e.getType());
            System.println("");
            System.println("Message: ");
            System.println(e.getMessage());
            System.println("");
        }
        
        System.println("Test complete");
    }
}