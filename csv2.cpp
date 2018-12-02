#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <opencv/cv.hpp>
#include "hough.cpp"

using namespace std;
using namespace cv;

String cascade_name = "dartcascade/cascade.xml";
CascadeClassifier cascade;

vector<vector<string> > readCSV(string filename) {
    vector<vector<string> > M;
    ifstream in(filename);
    string line;
    while (getline(in, line)) {                  // read a whole line of the file
        stringstream ss(line);                   // put it in a stringstream (internal stream)
        vector<string> row;
        string data;
        while (getline(ss, data, ',')) {         // read (string) items up to a comma
            row.push_back(data);                 // use stod() to convert to double; put in row vector
        }
        if (row.size() > 0) M.push_back(row);    // add non-empty rows to matrix
    }
    return M;
}

void write(const vector<vector<string> > &M) {
    const int w = 12;
    for (int i = 0; i < M.size(); i++) {
        for (int j = 0; j < M.at(i).size(); j++) {
            cout << setw(w) << M.at(i).at(j) << ' ';
        }
        cout << '\n';
    }
}

vector<vector<Rect> > find_truth_rects(vector<vector<string> > data) {
    vector<vector<Rect> > rects;
    for (int i = 0; i < data.size(); i++) {
        int num_rects = (data.at(i).size() - 1) / 4;
        vector<Rect> line;
        for (int j = 0; j < num_rects; j++) {
            int x = stoi(data.at(i).at((j*4)+1));
            int y = stoi(data.at(i).at((j*4)+2));
            int w = stoi(data.at(i).at((j*4)+3));
            int h = stoi(data.at(i).at((j*4)+4));
            Rect rect(x, y, w, h);
            line.push_back(rect);
        }
        rects.push_back(line);
    }
    return rects;
}

vector<Rect> detected_rects_per_image(Mat frame) {
    vector<Rect> faces;
    Mat frame_gray;
    cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
    cascade.detectMultiScale(frame_gray, faces, 1.1, 1, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50), Size(500,500));
    return faces;
}

vector<vector<Rect> > find_detected_rects(vector<vector<string> > data) {
    vector<vector<Rect> > rects;
    Mat frame;
    for (int i = 0; i < data.size(); i++) {
        string filename = "darts/" + data.at(i).at(0);
        frame = imread(filename, CV_LOAD_IMAGE_COLOR);
        rects.push_back(detected_rects_per_image(frame));
    }
    return rects;
}

void write_truth_images(vector<vector<string> > data) {
    vector<vector<Rect> > truth_rects = find_truth_rects(data);
    for (int i = 0; i < truth_rects.size(); i++) {
        string filename = "darts/" + data.at(i).at(0);
        Mat frame = imread(filename, CV_LOAD_IMAGE_COLOR);
        for (int j = 0; j < truth_rects.at(i).size(); j++) {
            Rect rect = truth_rects.at(i).at(j);
            rectangle(frame, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(255, 255, 0), 2);
        }
        imwrite("subtask2.1/truth" + data.at(i).at(0), frame);
    }
}

void write_detected_images(vector<vector<string> > data) {
    vector<vector<Rect> > detected_rects = find_detected_rects(data);
    for (int i = 0; i < detected_rects.size(); i++) {
        string filename = "darts/" + data.at(i).at(0);
        Mat frame = imread(filename, CV_LOAD_IMAGE_COLOR);
        for (int j = 0; j < detected_rects.at(i).size(); j++) {
            Rect rect = detected_rects.at(i).at(j);
            rectangle(frame, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(0, 255, 0), 2);
        }
        imwrite("subtask2.1/det" + data.at(i).at(0), frame);
    }
}

void write_images(vector<vector<string> > data, vector<vector<Rect> > rect_vector) {
    for (int i = 0; i < rect_vector.size(); i++) {
        string filename = "darts/" + data.at(i).at(0);
        Mat frame = imread(filename, CV_LOAD_IMAGE_COLOR);
        for (int j = 0; j < rect_vector.at(i).size(); j++) {
            Rect rect = rect_vector.at(i).at(j);
            rectangle(frame, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(0, 255, 0), 2);
        }
        imwrite("subtask3/filtered" + data.at(i).at(0), frame);
    }
}

void write_hough_info(string image, vector<Circle> circles, vector<Line> lines, vector<Rect> rects) {
    Mat frame = imread("darts/" + image, CV_LOAD_IMAGE_COLOR);
    for (int i = 0; i < circles.size(); i++) {
        Circle c = circles.at(i);
        circle(frame, c.getCenter(), c.radius, Scalar(255, 0, 255), 2);
    }
    for (int i = 0; i < lines.size(); i++) {
        double m = lines.at(i).m;
        double c = lines.at(i).c;
        Point p1(200, round(m * 200 + c));
        Point p2(300, round(m * 300 + c));
        //fullLine(frame, p1, p2, Scalar(255, 255, 0), m);
    }
    for (int i = 0; i < rects.size(); i++) {
        rectangle(frame, rects.at(i), Scalar(0, 255, 0), 2);
    }
    imwrite("subtask3/everything" + image, frame);
}

void write_both_images(vector<vector<string> > data) {
    vector<vector<Rect> > truth_rects = find_truth_rects(data);
    vector<vector<Rect> > detected_rects = find_detected_rects(data);
    for (int i = 0; i < truth_rects.size(); i++) {
        string filename = "darts/" + data.at(i).at(0);
        Mat frame = imread(filename, CV_LOAD_IMAGE_COLOR);
        for (int j = 0; j < truth_rects.at(i).size(); j++) {
            Rect rect = truth_rects.at(i).at(j);
            rectangle(frame, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(255, 255, 0), 2);
        }
        for (int j = 0; j < detected_rects.at(i).size(); j++) {
            Rect rect = detected_rects.at(i).at(j);
            rectangle(frame, Point(rect.x, rect.y), Point(rect.x + rect.width, rect.y + rect.height), Scalar(0, 255, 0), 2);
        }
        imwrite("subtask2.1/both" + data.at(i).at(0), frame);
    }
}

double percentage_overlap(Rect truth, Rect detected) {
    Rect intersect = truth & detected;
    double biggest_area = max(truth.area(), detected.area());
    double intersect_area = intersect.area();
    double overlap = intersect_area/biggest_area;
    return overlap;
}

double one_way_overlap(Rect bigger, Rect smaller) {
    Rect intersect = bigger & smaller;
    double intersect_area = intersect.area();
    double smaller_area = smaller.area();
    double overlap = intersect_area/smaller_area;
    return overlap;
}

double f1score(double true_positives, double false_postives, double false_negatives) {
    double precision = true_positives / (true_positives + false_postives);
    double recall = true_positives / (true_positives + false_negatives);
    if (true_positives ==  0) return 0;
    double f1 = 2 * precision * recall / (precision + recall);
    return f1;
}

vector<vector<double> > calc_f1scores(vector<vector<Rect> > truth_rects, vector<vector<Rect> > detected_rects) {
    vector<vector<double> > f1scores;
    for (int i = 0; i < detected_rects.size(); i++) {
        int true_positives = 0;
        for (int j = 0; j < truth_rects.at(i).size(); j++) {
            double max_overlap = 0.0;
            for (int k = 0; k < detected_rects.at(i).size(); k++) {
                Rect truth = truth_rects.at(i).at(j);
                Rect detected = detected_rects.at(i).at(k);
                double overlap = percentage_overlap(truth, detected);
                if (overlap > max_overlap) max_overlap = overlap;
            }
            if (max_overlap >= 0.45) true_positives++;
        }
        int false_positives = detected_rects.at(i).size() - true_positives;
        int false_negatives = truth_rects.at(i).size() - true_positives;
        double f1 = f1score(true_positives, false_positives, false_negatives);
        vector<double> row;
        row.push_back(true_positives);
        row.push_back(false_positives);
        row.push_back(false_negatives);
        row.push_back(f1);
        f1scores.push_back(row);
    }
    return f1scores;
}

void print_f1scores(vector<vector<double> > f1scores) {
    for (int i = 0; i < f1scores.size(); i++) {
        cout << i << ": ";
        for (int j = 0; j < f1scores.at(i).size(); j++) cout << f1scores.at(i).at(j) << " ";
        cout << "\n";
    }
    double sum = 0;
    for (int i = 0; i < f1scores.size(); i++) sum += f1scores.at(i).at(3);
    double ave = sum / f1scores.size();
    cout << "Average f1 score: " << ave << "\n";
}

Point intersection(int m0, int m1, int c0, int c1)  {
    if(m0-m1==0)  return Point(-1, -1);
    int x =(c1-c0)/(m0-m1);
    int y = m0*x+c0;
    return Point(x, y);
}

vector<Rect> filterRects(vector<Rect> detectedRects, vector<Circle> circles, vector<Line> lines)  {
    printf("Circle size: %lu  Lines Size: %lu  Det Rect Size: %lu\n", circles.size(), lines.size(), detectedRects.size());
    vector<Rect> filteredRects1;
    for (int i = 0; i < detectedRects.size(); i++)  {
        for (int c=0; c < circles.size(); c++)  {
            if (abs(detectedRects.at(i).x + detectedRects.at(i).width/2 - circles.at(c).x) < 20 &&
                abs(detectedRects.at(i).y + detectedRects.at(i).height/2     - circles.at(c).y) < 20)  {
                    filteredRects1.push_back(detectedRects.at(i));
                    i++;
                    c=-1;
            }
        }
    }
    printf("Circle size: %lu  Lines Size: %lu  Det Rect Size: %lu\n", circles.size(), lines.size(), filteredRects1.size());

    int intersectionCounts[filteredRects1.size()];
    for (int i = 0; i < filteredRects1.size(); i++)  intersectionCounts[i] = 0;

    for (int i = 0; i < filteredRects1.size(); i++)  {
        for (int l=0; l < lines.size(); l++)  {
            for (int l1=1; l1 < lines.size(); l1++)  {
                 if (filteredRects1.at(i).contains(
                     intersection(lines.at(l).m,lines.at(l1).m, lines.at(l).c, lines.at(l1).c)))
                    intersectionCounts[i]++;
            }
        }
    }
    vector<Rect> filteredRects2;

    for (int r = 0; r < filteredRects1.size(); r++)  {
        if(intersectionCounts[r] > 2)  {
            filteredRects2.push_back(filteredRects1.at(r));
        }
    }
    printf("Circle size: %lu  Lines Size: %lu  Det Rect Size: %lu\n", circles.size(), lines.size(), filteredRects2.size());

    return filteredRects2   ;
}

double cross_product( Point a, Point b ){
   return a.x*b.y - a.y*b.x;
}

double distance_to_line( Point begin, Point end, Point x ){
   //translate the begin to the origin
   end -= begin;
   x -= begin;

   //¿do you see the triangle?
   double area = cross_product(x, end);
   return abs(area / norm(end));
}

vector<Rect> remove_duplicate_rects(vector<Rect> rects) {
    cout << "input amount of rects: " << rects.size() << "\n";
    vector<Rect> no_duplicates;
    for (int i = 0; i < rects.size(); i++) {
        bool no_identicals = true;
        Rect r1 = rects.at(i);
        for (int j = 0; j < no_duplicates.size() && !no_identicals; j++) {
            Rect r2 = no_duplicates.at(j);
            if (r1 == r2) no_identicals = false;
        }
        no_duplicates.push_back(r1);
    }
    cout << "output amount of rects: " << no_duplicates.size() << "\n";
    return no_duplicates;
}

vector<Rect> update_detections(vector<Rect> detected_rects, vector<Circle> circles, vector<Line> lines) {

    detected_rects = remove_duplicate_rects(detected_rects);

    vector<int> circle_votes, line_votes, line_votes2;
    int inner_circle_of[circles.size()];
    for (int c = 0; c < circles.size(); c++) inner_circle_of[c] = -1;

    for (int c0 = 0; c0 < circles.size(); c0++) {
        for (int c1 = 0; c1 < circles.size(); c1++) {
            if (c0 != c1 && circles.at(c0).isInnerCircleOf(circles.at(c1))) {
                inner_circle_of[c1] = c0;
            }
        }
    }

    for (int c = 0; c < circles.size(); c++) cout << "inner circle of " << c << " is " << inner_circle_of[c] << "\n";

    for (int c0 = 0; c0 < circles.size(); c0++) {
        for (int c1 = 0; c1 < circles.size(); c1++) {
            if (c0 != c1 && inner_circle_of[c1] == c0 && inner_circle_of[c0] == -1) {
                cout << "adding outer circle: " << c1 << "\n";
                detected_rects.push_back(circles.at(c1).makeRect());
            }
        }
    }

    //Voting for rectangles that have an outer or inner circle
    for (int i = 0; i < detected_rects.size(); i++) {
        int vote = 0;
        for (int c = 0; c < circles.size(); c++) {
            if (circles.at(c).isSimilarTo(detected_rects.at(i))) {
                    vote++;
                    if (inner_circle_of[c] != -1 && inner_circle_of[inner_circle_of[c]] == -1) {
                        vote++;
                    }
            }
        }
        circle_votes.push_back(vote);
    }
    //Count lines passing through/near rect center
    for (int i = 0; i < detected_rects.size(); i++) {
        int vote = 0;
        vector<Line> linesNearCenter;
        Rect r = detected_rects.at(i);
        for (int l = 0; l < lines.size(); l++) {
            if(distance_to_line(Point(r.x, lines.at(l).getY(r.x)),
                                Point(r.x+r.width, lines.at(l).getY(r.x+r.width)),
                                Point(r.x+r.width/2, r.y+r.height/2)) < 10)
                linesNearCenter.push_back(lines.at(l));
        }
        int gradients[11];
        for (int g = 0; g < 11; g++) gradients[g] = 0;
        // printf("Lines near center %lu\n", linesNearCenter.size());
        for (int l = 0; l < linesNearCenter.size(); l++)  {
            gradients[(int)(atan(linesNearCenter.at(l).m)*10.0/M_PI+5.0)] = 1;
        }
        int sum = 0;
        for (int g = 0; g < 11; g++)  sum += gradients[g];
        printf("Diff line gradients %d\n", sum);
        line_votes2.push_back(sum);
    }



    // //Voting for rectangles that have an intersection near the center
    // for (int i = 0; i < detected_rects.size(); i++) {
    //     int vote = 0;
    //     for (int l1 = 0; l1 < lines.size(); l1++) {
    //         for (int l2 = 0; l2 < lines.size(); l2++) {
    //             if (l1 != l2) {
    //                 int allowedError = 0.4*detected_rects.at(i).width/2;
    //                 Point rect_center = Point(detected_rects.at(i).x + detected_rects.at(i).width/2,
    //                                           detected_rects.at(i).y + detected_rects.at(i).height/2);
    //                 Point intersect = intersection(lines.at(l1).m,lines.at(l2).m, lines.at(l1).c, lines.at(l2).c);
    //                 if (abs(rect_center.x - intersect.x) < allowedError && abs(rect_center.y - intersect.y) < allowedError/2) {
    //                     vote++;
    //                 }
    //             }
    //         }
    //     }
    //     line_votes.push_back(vote);
    // }

    vector<Rect> final_detections;

    //Vote counting
    for (int i = 0; i < detected_rects.size(); i++) {
        printf("Rectangle votes\n Circle %d\n Lines2 %d\n", circle_votes.at(i), line_votes2.at(i));
        if (circle_votes.at(i) +line_votes2.at(i)/2 > 1) {
            final_detections.push_back(detected_rects.at(i));
        }
    }


    for(int r1 = 0; r1 < final_detections.size(); r1++)  {
        for(int r2 = 0; r2 < final_detections.size(); r2++)  {
            if (final_detections.at(r1).area() > final_detections.at(r2).area()) {
                double overlap = one_way_overlap(final_detections.at(r1), final_detections.at(r2));
                printf("%f\n", overlap);
                if(overlap > 0.8)  final_detections.erase(final_detections.begin() + r2);
            }
        }
    }

    return final_detections;
}

int main(int n, char **args) {
    if (!cascade.load(cascade_name)) printf("--(!)Error loading\n");
    vector<vector<string> > data = readCSV("data.csv");
    vector<vector<Rect> > truth_rects = find_truth_rects(data);
    vector<vector<Rect> > detected_rects = find_detected_rects(data);

    Mat grad_dir, grad_mag;

    vector<vector<Rect> > new_rects;
    for (int i = 0; i < data.size(); i++) {
        string image_name = "darts/" + data.at(i).at(0);
        Mat image = imread(image_name, IMREAD_GRAYSCALE);
        getGradients(image, grad_mag, grad_dir);
        vector<Line> lines = houghTransformLines(image, grad_mag, grad_dir);
        vector<Circle> circles = HoughTransformCircles(image, grad_mag, grad_dir);
        vector<Rect> filtered_rects = update_detections(detected_rects.at(i), circles, lines);

        write_hough_info(data.at(i).at(0), circles, lines, filtered_rects);

        new_rects.push_back(filtered_rects);
        cout << "image " << i << " done.\n";
    }

    detected_rects = find_detected_rects(data);
    vector<vector<double> > original_f1scores = calc_f1scores(truth_rects, detected_rects);
    vector<vector<double> > filtered_f1scores = calc_f1scores(truth_rects, new_rects);

    print_f1scores(original_f1scores);
    print_f1scores(filtered_f1scores);

    write_images(data, new_rects);
    // vector<Circle> circles;
    // vector<Line> lines;
    //cout << lines->size() <<"\n";
    // write_truth_images(data);
    // write_detected_images(data);
    // write_both_images(data);
}
