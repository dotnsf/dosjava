class Point {
    int x;
    int y;
    
    void setX(int x) {
        this.x = x;
    }
    
    void setY(int y) {
        this.y = y;
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

// Made with Bob
