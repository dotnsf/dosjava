// ssocket.jav
class SSocket {
    public static void main(String args[]) {
        //ポート番号
        int PORT = 10000;  

        String str;
            
        // サーバソケットを作成
        ServerSocket ss = new ServerSocket(PORT);

        while(true) {
            // クライアントから要求を受ける
            Socket s = ss.accept();

            // クライアントからの文字列を読み取る
            InputStream is = s.getInputStream();
            BufferedReader br = is.BufferedReader();                
            str = br.readLine();

            System.out.println(str);

             // BufferedReaderオブジェクトをクローズする
            br.close();
        }
    }
}
