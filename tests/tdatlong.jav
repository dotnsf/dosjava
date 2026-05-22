// Test Date class with long type support
// IMPORTANT: Due to 32-bit limitations, getTime()/setTime() use SECONDS (not milliseconds)
// This allows dates up to year 2106

class TestDateLong {
    public static void main() {
        System.out.println("=== Date Long Type Test ===");
        System.out.println("Note: getTime()/setTime() use SECONDS (not milliseconds)");
        
        // Test 1: Create Date with current time
        System.out.println("\nTest 1: Date with current time");
        Date d1 = new Date();
        long t1 = d1.getTime();
        System.out.print("Current time (seconds): ");
        System.out.println(t1);
        System.out.print("Year: ");
        System.out.println(d1.getFullYear());
        System.out.print("Month: ");
        System.out.println(d1.getMonth());
        System.out.print("Day: ");
        System.out.println(d1.getDate());
        
        // Test 2: Create Date with specific timestamp
        System.out.println("\nTest 2: Date with specific timestamp");
        long timestamp = 3567587L;  // ~41 days from epoch (Feb 11, 1970)
        Date d2 = new Date(timestamp);
        long t2 = d2.getTime();
        System.out.print("Set time (seconds): ");
        System.out.println(t2);
        System.out.print("Year: ");
        System.out.println(d2.getFullYear());
        System.out.print("Month: ");
        System.out.println(d2.getMonth());
        System.out.print("Day: ");
        System.out.println(d2.getDate());
        
        // Test 3: setTime() method
        System.out.println("\nTest 3: setTime() method");
        Date d3 = new Date();
        long newTime = 1791994L;  // ~20 days from epoch (Jan 21, 1970)
        d3.setTime(newTime);
        long t3 = d3.getTime();
        System.out.print("After setTime (seconds): ");
        System.out.println(t3);
        System.out.print("Year: ");
        System.out.println(d3.getFullYear());
        System.out.print("Month: ");
        System.out.println(d3.getMonth());
        System.out.print("Day: ");
        System.out.println(d3.getDate());
        
        // Test 4: Second precision
        System.out.println("\nTest 4: Second precision");
        long sec1 = 1912276L;  // ~22 days from epoch (Jan 23, 1970)
        Date d4 = new Date(sec1);
        long sec2 = d4.getTime();
        System.out.print("Original: ");
        System.out.println(sec1);
        System.out.print("Retrieved: ");
        System.out.println(sec2);
        if (sec1 == sec2) {
            System.out.println("Second precision: OK");
        } else {
            System.out.println("Second precision: FAILED");
        }
        
        // Test 5: Date arithmetic
        System.out.println("\nTest 5: Date arithmetic");
        Date d5 = new Date();
        long t5 = d5.getTime();
        long oneDay = 86400L;  // 24 * 60 * 60 seconds
        long tomorrow = t5 + oneDay;
        d5.setTime(tomorrow);
        System.out.print("Today (seconds): ");
        System.out.println(t5);
        System.out.print("Tomorrow (seconds): ");
        System.out.println(d5.getTime());
        
        System.out.println("\n=== All tests completed ===");
    }
}
