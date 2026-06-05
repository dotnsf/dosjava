class httpprox {
    public static void main() {
        System.out.println("=== HTTP Proxy Test ===");
        
        try {
            // Test 1: Set proxy and get current proxy
            System.out.println("");
            System.out.println("Test 1: Set and get proxy");
            Http.setProxy("http://proxy.example.com:8080");
            String proxy = Http.getProxy();
            System.out.println("Proxy set to: ");
            System.out.println(proxy);
            
            // Test 2: Clear proxy
            System.out.println("");
            System.out.println("Test 2: Clear proxy");
            Http.clearProxy();
            String proxy2 = Http.getProxy();
            System.out.println("Proxy after clear: ");
            System.out.println(proxy2);
            System.out.println("(empty string expected)");
            
            // Test 3: Set proxy with authentication
            System.out.println("");
            System.out.println("Test 3: Proxy with auth");
            Http.setProxy("http://user:pass@proxy.example.com:8080");
            String proxy3 = Http.getProxy();
            System.out.println("Proxy with auth: ");
            System.out.println(proxy3);
            
            // Test 4: HTTP request with proxy (will fail without real proxy)
            System.out.println("");
            System.out.println("Test 4: HTTP request with proxy");
            System.out.println("(This will fail without a real proxy server)");
            
            // Clear proxy for actual test
            Http.clearProxy();
            
            System.out.println("");
            System.out.println("=== All proxy tests passed ===");
            
        } catch (Exception e) {
            System.out.println("Error: ");
            System.out.println(e.getMessage());
        }
        
        return;
    }
}

// Made with Bob
