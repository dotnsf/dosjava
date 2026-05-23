// Test: Switch with many cases (performance test)
class SwitchMany {
    public static void main() {
        int x;
        
        System.out.println("Testing switch with many cases");
        
        // Test with 6 cases
        x = 1;
        System.out.print("x=1: ");
        switch (x) {
            case 1: System.out.println("One"); break;
            case 2: System.out.println("Two"); break;
            case 3: System.out.println("Three"); break;
            case 4: System.out.println("Four"); break;
            case 5: System.out.println("Five"); break;
            case 6: System.out.println("Six"); break;
            default: System.out.println("Other"); break;
        }
        
        x = 6;
        System.out.print("x=6: ");
        switch (x) {
            case 1: System.out.println("One"); break;
            case 2: System.out.println("Two"); break;
            case 3: System.out.println("Three"); break;
            case 4: System.out.println("Four"); break;
            case 5: System.out.println("Five"); break;
            case 6: System.out.println("Six"); break;
            default: System.out.println("Other"); break;
        }
        
        x = 99;
        System.out.print("x=99: ");
        switch (x) {
            case 1: System.out.println("One"); break;
            case 2: System.out.println("Two"); break;
            case 3: System.out.println("Three"); break;
            case 4: System.out.println("Four"); break;
            case 5: System.out.println("Five"); break;
            case 6: System.out.println("Six"); break;
            default: System.out.println("Other"); break;
        }
        
        System.out.println("Many cases test complete");
        return;
    }
}