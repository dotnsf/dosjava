class httpprox {
    public static void main() {
        // Test 1: Set proxy and verify
        Http.setProxy("http://192.168.0.81:3128");
        String proxy1 = Http.getProxy();
        int test1 = proxy1.equals("http://192.168.0.81:3128");
        
        // Test 2: Get contents via proxy (use HTTP instead of HTTPS to avoid IPv6 issues)
        String html = Http.get("http://example.com/");
        
        // Check if response contains error or Squid error page
        int errorIdx = html.indexOf("ERR_");
        int exampleIdx = html.indexOf("Example Domain");
        
        // Test 3: Clear proxy and verify empty
        Http.clearProxy();
        String proxy2 = Http.getProxy();
        int test3 = proxy2.equals("");
        
        // Verify all tests passed
        // Success only if: proxy set correctly, no Squid error, example.com content found, proxy cleared
        if (test1 == 1 && errorIdx < 0 && exampleIdx >= 0 && test3 == 1) {
            System.out.println("httpprox.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
