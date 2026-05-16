class dtest3 {
    public static void main() {
        Date d = new Date();
        
        int year = d.getFullYear();
        int month = d.getMonth();
        int day = d.getDate();
        int hours = d.getHours();
        int minutes = d.getMinutes();
        int seconds = d.getSeconds();
        
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
        
        return;
    }
}
