class HttpTest {
    // Simple delay function using busy loop
    public static void delay(int milliseconds) {
        int iterations = milliseconds * 100;  // Approximate calibration
        int dummy = 0;
        for (int i = 0; i < iterations; i = i + 1) {
            dummy = dummy + 1;
        }
        return;
    }
    
    public static void main() {
        boolean allPassed = true;
        
        // Test 1: Basic GET request (without headers)
        try {
            String response1 = Http.get("http://192.168.0.2:8080/");
            if (response1 == null || response1.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        // Wait between requests to avoid timeout
        delay(2000);
        
        // Test 2: GET request with single header
        try {
            String headers1 = "User-Agent: DOSJava/1.0";
            String response2 = Http.get("http://192.168.0.2:8080/", headers1);
            if (response2 == null || response2.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);
        
        // Test 3: GET request with multiple headers
        try {
            String headers2 = "User-Agent: DOSJava/1.0\nAccept: application/json";
            String response3 = Http.get("http://192.168.0.2:8080/", headers2);
            if (response3 == null || response3.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);
        
        // Test 4: POST request with data (no headers)
        try {
            String postData = "name=DOSJava&version=1.0";
            String response4 = Http.post("http://192.168.0.2:8080/", postData);
            if (response4 == null || response4.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);
        
        // Test 5: POST request with data and headers
        try {
            String postData2 = "test=phase4.7";
            String headers3 = "Content-Type: application/x-www-form-urlencoded";
            String response5 = Http.post("http://192.168.0.2:8080/", postData2, headers3);
            if (response5 == null || response5.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);
        
        // Test 6: PUT request (no headers)
        try {
            String putData = "updated=true";
            String response6 = Http.put("http://192.168.0.2:8080/", putData);
            if (response6 == null || response6.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);
        
        // Test 7: PUT request with headers
        try {
            String putData2 = "status=active";
            String headers4 = "X-Custom-Header: Phase4.7";
            String response7 = Http.put("http://192.168.0.2:8080/", putData2, headers4);
            if (response7 == null || response7.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);
        
        // Test 8: DELETE request (no headers)
        try {
            String response8 = Http.delete("http://192.168.0.2:8080/");
            if (response8 == null || response8.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        delay(2000);
        
        // Test 9: DELETE request with headers
        try {
            String headers5 = "Authorization: Bearer token123";
            String response9 = Http.delete("http://192.168.0.2:8080/", headers5);
            if (response9 == null || response9.length() == 0) {
                allPassed = false;
            }
        } catch (Exception e) {
            allPassed = false;
        }
        
        // Output result
        if (allPassed) {
            System.out.println("http.jav worked correctly.");
        }
        
        return;
    }
}
