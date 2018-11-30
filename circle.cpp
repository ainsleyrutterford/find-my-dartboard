#include <iostream>
#include <opencv/cv.hpp>

using namespace std;
using namespace cv;

class Circle {
    private:
        const double percentage = 0.4;
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
        int allowedError = percentage*radius;
        if (abs( otherCircle.x - x) > allowedError) return false;
        if (abs( otherCircle.y - y) > allowedError) return false;
        if (abs( otherCircle.radius - radius) > allowedError) return false;
        return true;
    }

    bool isSimilarTo(Rect r)  {
        int allowedError = percentage*radius;

        if(abs(r.x+r.width/2 - x) > allowedError)  return false;
        if(abs(r.y+r.height/2 - y) > allowedError)  return false;
        if(abs(r.width/2 - radius) > allowedError || abs(0.63*(r.width/2) - radius) > allowedError)  return false;
        if(abs(r.height/2 - radius) > allowedError || abs(0.63*(r.height/2) - radius) > allowedError)  return false;
        return true;
    }

    bool isOuterCircleOf(Circle c)  {
        int allowedError = percentage*radius;
        if(abs(c.x- x) > allowedError)  return false;
        if(abs(c.y - y) > allowedError)  return false;
        if(abs(0.63*radius - c.radius) > allowedError)  return false;
        return true;
    }

    Rect makeRect()  {
         return Rect(x-radius, y-radius, 2*radius, 2*radius);
    }

};
