#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <opencv/cv.hpp>
#include "hough.cpp"
#include "DartImage.cpp"

using namespace std;
using namespace cv;

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

vector<DartImage> set_rects(vector<vector<string> > data) {
    vector<DartImage> dartImages;
    for (int i = 0; i < data.size(); i++) {
        int num_rects = (data.at(i).size() - 1) / 4;
        DartImage dartImage(data.at(i).at(0));
        for (int j = 0; j < num_rects; j++) {
            int x = stoi(data.at(i).at((j*4)+1));
            int y = stoi(data.at(i).at((j*4)+2));
            int w = stoi(data.at(i).at((j*4)+3));
            int h = stoi(data.at(i).at((j*4)+4));
            dartImage.addTruthRect(Rect(x, y, w, h));
            dartImage.setDetectedRects();

        }
        dartImages.push_back(dartImage);
    }
    return dartImages;
}

void write_images(vector<DartImage> dartImages) {
    for (int i = 0; i < dartImages.size(); i++) {
        string filename = "darts/" + dartImages.at(i).getImageName();
        Mat frame = imread(filename, CV_LOAD_IMAGE_COLOR);
        for (int j = 0; j < dartImages.at(i).getFilteredRects().size(); j++) {
            rectangle(frame, dartImages.at(i).getFilteredRects().at(j), Scalar(0, 255, 0), 2);
        }
        imwrite("subtask3/filtered" + dartImages.at(i).getImageName(), frame);
    }
}

void write_hough_info(string image_name, vector<Circle> circles, vector<Line> lines, vector<Rect> rects) {
    Mat frame = imread("darts/" + image_name, CV_LOAD_IMAGE_COLOR);

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
    imwrite("subtask3/everything" + image_name, frame);
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

vector<Rect> update_detections(vector<Rect> detected_rects, vector<Circle> circles, vector<Line> lines) {

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
    vector<vector<string> > ground_truth_string_data = readCSV("data.csv");
    vector<DartImage> dartImages = set_rects(ground_truth_string_data);

    Mat grad_dir, grad_mag;
    vector<vector<Rect> > new_rects;
    vector<double> origf1scores;
    vector<double> newf1scores;
    for (int i = 0; i < dartImages.size(); i++) {
        DartImage dartImage = dartImages.at(i);
        getGradients(dartImage.getImage(), grad_mag, grad_dir);
        vector<Line> lines = houghTransformLines(dartImage.getImage(), grad_mag, grad_dir);
        vector<Circle> circles = HoughTransformCircles(dartImage.getImage(), grad_mag, grad_dir);
        vector<Rect> filtered_rects = update_detections(dartImage.getDetectedRects(), circles, lines);
        write_hough_info(dartImage.getImageName(), circles, lines, filtered_rects);

        dartImage.setFilteredRects(filtered_rects);
        origf1scores.push_back(dartImage.calc_original_f1());
        newf1scores.push_back(dartImage.calc_new_f1());
        printf("Size of DR %lu\n", dartImage.getDetectedRects().size());
        printf("Size of FR  %lu\n", dartImage.getFilteredRects().size());
        printf("Size of TR %lu\n", dartImage.getTruthRects().size());
        cout << "image " << i << " done.\n";
    }

    double sumNewF1 = 0.0;
    double sumOrigF1 = 0.0;

    for (int i = 0; i < origf1scores.size(); i++)  {
        sumNewF1  += newf1scores.at(i);
        sumOrigF1 += origf1scores.at(i);
    }
    printf("Original F1 Score %f\n", sumOrigF1/origf1scores.size());
    printf("New F1 Score %f\n", sumNewF1/newf1scores.size());



    write_images(dartImages);
}
