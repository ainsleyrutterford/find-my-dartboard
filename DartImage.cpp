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

        double percentage_overlap(Rect truth, Rect detected) {
            Rect intersect = truth & detected;
            double biggest_area = max(truth.area(), detected.area());
            double intersect_area = intersect.area();
            double overlap = intersect_area/biggest_area;
            return overlap;
        }

        double f1score(double true_positives, double false_positives, double false_negatives) {
            double precision = true_positives / (true_positives + false_positives);

            double recall = true_positives / (true_positives + false_negatives);
            if (true_positives == 0 || (true_positives + false_positives) == 0 || (true_positives + false_negatives) ==  0) return 0;
            double f1 = 2 * precision * recall / (precision + recall);
            return f1;
        }
        double recall_score(double true_positives, double false_positives, double false_negatives) {
            double recall = true_positives / (true_positives + false_negatives);
            if (true_positives == 0 || true_positives + false_negatives ==  0) return 0;

            return recall;
        }

        double precision_score(double true_positives, double false_positives, double false_negatives) {
            double precision = true_positives / (true_positives + false_positives);
            if (true_positives == 0 || true_positives + false_positives ==  0) return 0;
            return precision;
        }

        double both_f1_score(vector<Rect> rects)  {
            int true_positives = 0;
            for (int j = 0; j < truth_rects.size(); j++) {
                double max_overlap = 0.0;
                for (int k = 0; k < rects.size(); k++) {
                    double overlap = percentage_overlap(truth_rects.at(j), rects.at(k));
                    if (overlap > max_overlap) max_overlap = overlap;
                }
                if (max_overlap >= 0.45) true_positives++;
            }
            int false_positives = rects.size() - true_positives;
            int false_negatives = truth_rects.size() - true_positives;
            double f1 = f1score(true_positives, false_positives, false_negatives);
            return f1;
        }
        double both_recall_score(vector<Rect> rects)  {
            int true_positives = 0;
            for (int j = 0; j < truth_rects.size(); j++) {
                double max_overlap = 0.0;
                for (int k = 0; k < rects.size(); k++) {
                    double overlap = percentage_overlap(truth_rects.at(j), rects.at(k));
                    if (overlap > max_overlap) max_overlap = overlap;
                }
                if (max_overlap >= 0.45) true_positives++;
            }
            int false_positives = rects.size() - true_positives;
            int false_negatives = truth_rects.size() - true_positives;
            double recall = recall_score(true_positives, false_positives, false_negatives);
            return recall;
        }
        double both_precision_score(vector<Rect> rects)  {
            int true_positives = 0;
            for (int j = 0; j < truth_rects.size(); j++) {
                double max_overlap = 0.0;
                for (int k = 0; k < rects.size(); k++) {
                    double overlap = percentage_overlap(truth_rects.at(j), rects.at(k));
                    if (overlap > max_overlap) max_overlap = overlap;
                }
                if (max_overlap >= 0.45) true_positives++;
            }
            int false_positives = rects.size() - true_positives;
            int false_negatives = truth_rects.size() - true_positives;

            double precision = precision_score(true_positives, false_positives, false_negatives);
            return precision;
        }

    public:
        DartImage(string name) {
            this->name = name;
            image = imread("darts/" + this->name, IMREAD_GRAYSCALE);

        }
        DartImage(){

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
            if (!cascade.load(cascade_name)) printf("--(!)Error loading\n");
            Mat equalised;
            image.copyTo(equalised);

	        equalizeHist(image, equalised);
            cascade.detectMultiScale(equalised, detected_rects, 
                                     1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(500,500));
            this->detected_rects = detected_rects;
        }
        void setFilteredRects(vector<Rect> filtered_rects)  {
            this->filtered_rects = filtered_rects;
        }
        vector<Rect> getFilteredRects()  {
            return filtered_rects;
        }
        double calc_original_f1() {
            return both_f1_score(detected_rects);
        }
        double calc_new_f1() {
            return both_f1_score(filtered_rects);
        }
        double calc_original_recall() {
            printf("Orig recall %f\n", both_recall_score(detected_rects));
            return both_recall_score(detected_rects);
        }
        double calc_new_recall() {
            printf("New recall %f\n", both_recall_score(filtered_rects));
            return both_recall_score(filtered_rects);
        }

        double calc_original_precision() {
            printf("Orig precision %f\n", both_precision_score(detected_rects));
            return both_precision_score(detected_rects);
        }
        double calc_new_precision() {
            printf("New precision %f\n", both_precision_score(filtered_rects));
            return both_precision_score(filtered_rects);
        }

        
            
};