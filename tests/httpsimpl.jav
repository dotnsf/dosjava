class HttpSimpleTest {
    static void main() {
        System.println("Testing HTTP GET with single header...");
        
        try {
            // Test with simple single header
            String headers = "User-Agent: DOSJava";
            String response = Http.get("http://192.168.0.2:8080/", headers);
            System.println("Success! Response:");
            System.println(response);
            
        } catch (Exception e) {
            System.println("Error:");
            System.printInt(e.getType());
            System.println("");
            System.println(e.getMessage());
        }
        
        System.println("Done");
    }
}