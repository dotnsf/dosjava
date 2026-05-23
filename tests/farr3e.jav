class farr3e {
    public static void main() {
        float[] arr;
        float sum;
        float temp;
        
        arr = new float[3];
        arr[0] = 1.5f;
        arr[1] = 2.5f;
        arr[2] = 3.5f;
        
        System.out.println("Array values:");
        System.out.println(arr[0]);
        System.out.println(arr[1]);
        System.out.println(arr[2]);
        
        sum = 0.0f;
        System.out.println("Initial sum:");
        System.out.println(sum);
        
        temp = arr[0];
        System.out.println("temp = arr[0]:");
        System.out.println(temp);
        
        sum = sum + temp;
        System.out.println("sum = sum + temp:");
        System.out.println(sum);
        
        temp = arr[1];
        System.out.println("temp = arr[1]:");
        System.out.println(temp);
        
        sum = sum + temp;
        System.out.println("sum = sum + temp:");
        System.out.println(sum);
        
        return;
    }
}