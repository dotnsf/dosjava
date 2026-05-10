// csocket.jav
class CSocket {
    public static void main(String args[]) {
        int PORT = 10000;
        String LOCALHOST = "127.0.0.1";

        Socket s = new Socket(LOCALHOST, PORT);

        // 文字列をサーバに書き込む
        OutputStream os = s.getOutputStream();
        BufferedWriter bw = os.BufferedWriter();
        bw.write(args[0]);

        // BufferedWriterオブジェクトをクローズする
        bw.close();
    }
}
