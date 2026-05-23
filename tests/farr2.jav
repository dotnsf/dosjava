class farr2 {
    public static void main() {
        float[] arr;
        int i;
        
        arr = new float[5];
        
        arr[0] = 0.0f;
        arr[1] = 1.0f;
        arr[2] = 2.0f;
        arr[3] = 3.0f;
        arr[4] = 4.0f;
        
        i = 0;
        while (i < 5) {
            System.out.println(arr[i]);
            i = i + 1;
        }
        
        return;
    }
}
