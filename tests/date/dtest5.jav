class dtest5 {
    public static void main() {
        System.out.println("Test 5: Multiple Dates");
        System.out.println("");
        
        Date d1 = new Date();
        Date d2 = new Date(5000);
        Date d3 = new Date(15000);
        
        System.out.println("Date 1 (now):");
        System.out.println(d1.getFullYear());
        System.out.println(d1.getMonth());
        System.out.println(d1.getDate());
        
        System.out.println("");
        System.out.println("Date 2 (5000):");
        System.out.println(d2.getFullYear());
        System.out.println(d2.getMonth());
        System.out.println(d2.getDate());
        System.out.println(d2.getHours());
        System.out.println(d2.getMinutes());
        System.out.println(d2.getSeconds());
        
        System.out.println("");
        System.out.println("Date 3 (15000):");
        System.out.println(d3.getFullYear());
        System.out.println(d3.getMonth());
        System.out.println(d3.getDate());
        System.out.println(d3.getHours());
        System.out.println(d3.getMinutes());
        System.out.println(d3.getSeconds());
        
        System.out.println("");
        System.out.println("setTime(25000)");
        d2.setTime(25000);
        
        System.out.println("Date 2 after:");
        System.out.println(d2.getFullYear());
        System.out.println(d2.getMonth());
        System.out.println(d2.getDate());
        System.out.println(d2.getHours());
        System.out.println(d2.getMinutes());
        System.out.println(d2.getSeconds());
        
        System.out.println("");
        System.out.println("Test 5 OK");
        
        return;
    }
}
