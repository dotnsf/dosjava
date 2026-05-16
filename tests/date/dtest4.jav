class dtest4 {
    public static void main() {
        int timestamp = 10000;
        Date d = new Date(timestamp);
        
        int year = d.getFullYear();
        int month = d.getMonth();
        int day = d.getDate();
        int hours = d.getHours();
        int minutes = d.getMinutes();
        int seconds = d.getSeconds();
        
        System.out.println("Test 1: Date(10000)");
        System.out.println("Year: ");
        System.out.println(year);
        System.out.println("Month: ");
        System.out.println(month);
        System.out.println("Day: ");
        System.out.println(day);
        System.out.println("Hours: ");
        System.out.println(hours);
        System.out.println("Minutes: ");
        System.out.println(minutes);
        System.out.println("Seconds: ");
        System.out.println(seconds);
        
        System.out.println("Test 2: setTime(20000)");
        d.setTime(20000);
        
        year = d.getFullYear();
        month = d.getMonth();
        day = d.getDate();
        hours = d.getHours();
        minutes = d.getMinutes();
        seconds = d.getSeconds();
        
        System.out.println("Year: ");
        System.out.println(year);
        System.out.println("Month: ");
        System.out.println(month);
        System.out.println("Day: ");
        System.out.println(day);
        System.out.println("Hours: ");
        System.out.println(hours);
        System.out.println("Minutes: ");
        System.out.println(minutes);
        System.out.println("Seconds: ");
        System.out.println(seconds);
        
        System.out.println("Test 3: getTime()");
        int time = d.getTime();
        System.out.println("Time: ");
        System.out.println(time);
        
        return;
    }
}
