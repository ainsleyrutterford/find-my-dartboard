#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <opencv/cv.hpp>

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
            if (max_overlap >= 0.6) true_positives++;
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
}

int main() {
    if (!cascade.load(cascade_name)) printf("--(!)Error loading\n");
    vector<vector<string> > data = readCSV("data.csv");
    vector<vector<Rect> > truth_rects = find_truth_rects(data);
    vector<vector<Rect> > detected_rects = find_detected_rects(data);
    vector<vector<double> > f1scores = calc_f1scores(truth_rects, detected_rects);
    write_truth_images(data);
    write_detected_images(data);
    write_both_images(data);
    print_f1scores(f1scores);
}
