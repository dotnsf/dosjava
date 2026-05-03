// Switch Statement Test
class SwitchTest {
    public static void main() {
        int x = 1;
        int z = -1;

        switch(x){
        case 1:
            z = 10;
            break;
        case 2:
            z = 20;
            break;
        case 3:
            z = 30;
            break;
        default:
            z = 0;
        }
        
        System.out.println(z);
        return;
    }
}


