// Simple HTTP test - no exception handling
class HttpSimpleTest {
    static void main() {
        String response;
        
        System.println("Calling Http.get...");
        response = Http.get("http://example.com");
        System.println("Got response:");
        System.println(response);
        System.println("Done");
    }
}