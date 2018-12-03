#include <iostream>
#include <opencv/cv.hpp>

using namespace std;
using namespace cv;

class Ellipse {
    private:
    public:
    int x, y, ry, rx;

    Ellipse(int x, int y, int ry, int rx) {
        this->x = x;
        this->y = y;
        this->rx = rx;
        this->ry = ry;
    }

    RotatedRect getRect()  {
        return RotatedRect(Point(x - rx/2, y-ry/2),Size(rx, ry), 0);
    }
};