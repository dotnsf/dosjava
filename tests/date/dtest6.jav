class dtest6 {
    public static void main() {
        System.out.println("Test 6: Formatting");
        System.out.println("");
        
        Date d = new Date();
        
        int year = d.getFullYear();
        int month = d.getMonth();
        int day = d.getDate();
        int hours = d.getHours();
        int minutes = d.getMinutes();
        int seconds = d.getSeconds();
        
        System.out.println("Date:");
        System.out.println(year);
        System.out.println("-");
        
        if (month < 9) {
            System.out.println("0");
        }
        int m = month + 1;
        System.out.println(m);
        System.out.println("-");
        
        if (day < 10) {
            System.out.println("0");
        }
        System.out.println(day);
        
        System.out.println("");
        System.out.println("Time:");
        
        if (hours < 10) {
            System.out.println("0");
        }
        System.out.println(hours);
        System.out.println(":");
        
        if (minutes < 10) {
            System.out.println("0");
        }
        System.out.println(minutes);
        System.out.println(":");
        
        if (seconds < 10) {
            System.out.println("0");
        }
        System.out.println(seconds);
        
        System.out.println("");
        System.out.println("Test 6 OK");
        
        return;
    }
}
