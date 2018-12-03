#include <iostream>
#include <opencv/cv.hpp>

using namespace std;
using namespace cv;

class DartImage {
    private:
        string name;
        Mat image;
        vector<Rect> truth_rects, detected_rects, filtered_rects;
        String cascade_name = "dartcascade/cascade.xml";
        CascadeClassifier cascade;
    public:
        DartImage(string name) {
            this->name = name;
            image = imread(name, IMREAD_GRAYSCALE);

        }

        string getImageName()  {
            return name;
        }
        Mat getImage()  {
            return image;
        }
        vector<Rect> getTruthRects()  {
            return truth_rects;
        }
        void addTruthRect(Rect rect)  {
            truth_rects.push_back(rect);
        }
        vector<Rect> getDetectedRects()  {
            return detected_rects;
        }
        void setDetectedRects()  {
            Mat equalised;
	        equalizeHist(image, equalised);
            cascade.detectMultiScale(equalised, detected_rects, 
                                     1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(500,500));
            this->detected_rects = detected_rects;
        }
        void setFilteredRects(vector<Rect> filteredRects)  {
            filtered_rects = filteredRects;
        }
};