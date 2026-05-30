class HttpHeaderDebug {
    static void main() {
        System.println("Testing HTTP GET with custom headers (debug)...");
        System.println("");
        
        try {
            System.println("Test 1: GET with custom headers");
            String headers = "Content-Type: application/json\nUser-Agent: DOSJava/1.0";
            System.println("Headers: ");
            System.println(headers);
            System.println("");
            
            System.println("Calling Http.get with headers...");
            String response = Http.get("http://example.com/", headers);
            System.println("Success! Response:");
            System.println(response);
            System.println("");
            
        } catch (Exception e) {
            System.println("Exception caught!");
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
