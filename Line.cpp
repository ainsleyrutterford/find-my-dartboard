#include <iostream>
using namespace std;

class Line {
    private:
    public:
    double m, c;

    Line(double m, double c) {
        this->m = m;
        this->c = c;
    }

    int getY(int x)  {
        return m*x+c;
    }

    Point getIntersection(Line l1)  {
        if(m-l1.m==0)  return Point(-1, -1);
        int x =(l1.c-c)/(m-l1.m);
        int y = m*x+c;
        return Point(x, y);
    }
    double cross_product( Point a, Point b ){
        return a.x*b.y - a.y*b.x;
    }

    double distance_to_line( Rect r ){
        Point begin(r.x, getY(r.x));
        Point end(r.x+r.width, getY(r.x+r.width)),
        Point rect_center(r.x+r.width/2, r.y+r.height/2)

        //translate the begin to the origin
        end -= begin;
        rect_center -= begin;

        //¿do you see the triangle?
        double area = cross_product(rect_center, end);
        return abs(area / norm(end));
    }
};
