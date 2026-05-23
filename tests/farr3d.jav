class farr3d {
    public static void main() {
        float[] arr;
        
        arr = new float[3];
        System.out.println("After array creation");
        
        arr[0] = 1.5f;
        System.out.println("After arr[0] = 1.5f");
        System.out.println(arr[0]);
        
        arr[1] = 2.5f;
        System.out.println("After arr[1] = 2.5f");
        System.out.println(arr[1]);
        
        arr[2] = 3.5f;
        System.out.println("After arr[2] = 3.5f");
        System.out.println(arr[2]);
        
        System.out.println("All values:");
        System.out.println(arr[0]);
        System.out.println(arr[1]);
        System.out.println(arr[2]);
        
        return;
    }
}