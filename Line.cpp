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
};
