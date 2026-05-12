// bufrw.jav
class bufrw {
    public static void main() {
        FileOutputStream fos = new FileOutputStream("TEST.TXT");
        BufferedWriter bw = new BufferedWriter(fos);
        bw.write("Hello File");
        bw.close();
    }
}
