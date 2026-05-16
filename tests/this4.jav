class Point {
    int x;
    int y;
    
    void setX(int newX) {
        this.x = newX;
    }
    
    void setY(int newY) {
        this.y = newY;
    }
    
    int getX() {
        return this.x;
    }
    
    int getY() {
        return this.y;
    }
    
    public static void main() {
        Point p = new Point();
        p.setX(10);
        p.setY(20);
        System.out.println(p.getX());
        System.out.println(p.getY());
    }
}
