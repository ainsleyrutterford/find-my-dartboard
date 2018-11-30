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
};
