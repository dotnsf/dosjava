class farr3f {
    public static void main() {
        float[] arr;
        float sum;
        
        arr = new float[3];
        arr[0] = 1.5f;
        arr[1] = 2.5f;
        arr[2] = 3.5f;
        
        sum = 0.0f;
        System.out.println("Initial sum:");
        System.out.println(sum);
        
        System.out.println("arr[0]:");
        System.out.println(arr[0]);
        
        sum = sum + arr[0];
        System.out.println("After sum = sum + arr[0]:");
        System.out.println(sum);
        
        System.out.println("arr[1]:");
        System.out.println(arr[1]);
        
        sum = sum + arr[1];
        System.out.println("After sum = sum + arr[1]:");
        System.out.println(sum);
        
        return;
    }
}