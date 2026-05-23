class farr3s {
    public static void main() {
        float[] arr;
        float sum;
        
        arr = new float[3];
        arr[0] = 1.5f;
        arr[1] = 2.5f;
        arr[2] = 3.5f;
        
        sum = 0.0f;
        sum = sum + arr[0];
        System.out.println(sum);
        sum = sum + arr[1];
        System.out.println(sum);
        sum = sum + arr[2];
        System.out.println(sum);
        
        return;
    }
}
