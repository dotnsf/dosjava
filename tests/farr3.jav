class farr3 {
    public static void main() {
        float[] arr;
        float sum;
        int i;
        
        arr = new float[3];
        arr[0] = 1.5f;
        arr[1] = 2.5f;
        arr[2] = 3.5f;
        
        sum = 0.0f;
        i = 0;
        while (i < 3) {
            sum = sum + arr[i];
            i = i + 1;
        }
        
        System.out.println(sum);
        
        return;
    }
}
