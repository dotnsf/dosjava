class ExpLogTest {
    public static void main() {
        // Test exp()
        float exp0 = Math.exp(0.0f);        // ~1.0
        float exp1 = Math.exp(1.0f);        // ~2.718 (e)
        float exp2 = Math.exp(2.0f);        // ~7.389
        
        // Test log() (natural logarithm)
        float log1 = Math.log(1.0f);        // ~0.0
        float loge = Math.log(2.718f);      // ~1.0
        float log10 = Math.log(10.0f);      // ~2.303
        
        // Verify results (with tolerance for floating point)
        boolean check1 = (exp0 > 0.99f && exp0 < 1.01f);
        boolean check2 = (exp1 > 2.71f && exp1 < 2.72f);
        boolean check3 = (exp2 > 7.38f && exp2 < 7.40f);
        boolean check4 = (log1 > -0.01f && log1 < 0.01f);
        boolean check5 = (loge > 0.99f && loge < 1.01f);
        boolean check6 = (log10 > 2.30f && log10 < 2.31f);
        
        if (check1 && check2 && check3 && check4 && check5 && check6) {
            System.out.println("explog.jav worked correctly.");
        }
        
        return;
    }
}
