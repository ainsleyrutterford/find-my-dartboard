#include <iostream>
#include <opencv/cv.hpp>

using namespace std;
using namespace cv;

class Circle {
    private:
    public:
    int x, y, radius;

    Circle(int x, int y, int r) {
        this->x = x;
        this->y = y;
        this->radius = r;
    }

    Point getCenter() {
        return Point(x,y);
    }

    bool isSimilarTo(Circle otherCircle)  {
        if (abs( otherCircle.x - x) < 5 &&
            abs( otherCircle.y - y) < 5 &&
            abs( otherCircle.radius - radius) < 5)
            return true;
        return false;
    }

};
