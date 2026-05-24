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
        
        // Test log10() (base 10 logarithm)
        float log10_1 = Math.log10(1.0f);   // ~0.0
        float log10_10 = Math.log10(10.0f); // ~1.0
        float log10_100 = Math.log10(100.0f); // ~2.0
        
        // Verify results (with tolerance for floating point)
        boolean check1 = (exp0 > 0.99f && exp0 < 1.01f);
        boolean check2 = (exp1 > 2.71f && exp1 < 2.72f);
        boolean check3 = (exp2 > 7.38f && exp2 < 7.40f);
        boolean check4 = (log1 > -0.01f && log1 < 0.01f);
        boolean check5 = (loge > 0.99f && loge < 1.01f);
        boolean check6 = (log10 > 2.30f && log10 < 2.31f);
        boolean check7 = (log10_1 > -0.01f && log10_1 < 0.01f);
        boolean check8 = (log10_10 > 0.99f && log10_10 < 1.01f);
        boolean check9 = (log10_100 > 1.99f && log10_100 < 2.01f);
        
        if (check1 && check2 && check3 && check4 && check5 && 
            check6 && check7 && check8 && check9) {
            System.out.println("explog.jav worked correctly.");
        }
        
        return;
    }
}

// Made with Bob
