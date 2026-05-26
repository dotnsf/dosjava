class Fibonacci{
  public static void main(){
    int n = 10;
    int a = 0;
    int b = 1;
    int i = 0;
    
    while( i < n ){
      int t = a + b;
      a = b;
      b = t;
      System.out.println( b );
      i ++;
    }
    
    return;
  }
}
