#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "face.cpp"
#include "f1score.cpp"
#include <cmath>
using namespace std;
using namespace cv;

//==========================================================


vector<vector<string> > readCSV( string filename ) {
    vector<vector<string> > M;

    ifstream in( filename );
    string line;
    while ( getline( in, line ) ) {                 // read a whole line of the file
        stringstream ss( line );                     // put it in a stringstream (internal stream)
        vector<string> row;
        string data;
        while ( getline( ss, data, ',' ) ) {         // read (string) items up to a comma
            row.push_back( data );            // use stod() to convert to double; put in row vector
        }
        if ( row.size() > 0 ) M.push_back( row );    // add non-empty rows to matrix
    }
    return M;
}


//==========================================================


void write( const vector<vector<string> > &M ) {
    const int w = 12;
// cout << fixed;

    for (int i = 0; i < M.size(); i++) {
        for (int j = 0; j < M.at(i).size(); j++) cout << setw( w ) << M.at(i).at(j) << ' ';
        cout << '\n';
    }
}


//==========================================================

void drawrect(vector<string> dart_box)  {
    Mat frame = imread("darts/"+dart_box.at(0), CV_LOAD_IMAGE_COLOR);

    for (int i = 1; i < dart_box.size(); i+=4)  {
        int x = stoi(dart_box.at(i));
        int y = stoi(dart_box.at(i+1));
        int w = stoi(dart_box.at(i+2));
        int h = stoi(dart_box.at(i+3));
        rectangle(frame, Point(x, y), Point(x+w, y+h), Scalar(255, 255, 0), 2);
    }


    imwrite("subtask2/t" + dart_box.at(0), frame);

}

bool compareRect(Rect rect1, Rect rect2)  {
    if(abs(rect1.x - rect2.x) > 75)  return false;
    if(abs(rect1.y - rect2.y) > 75)  return false;
    if(abs(rect1.height - rect2.height) > 75)  return false;
    if(abs(rect1.width - rect2.width) > 75)  return false;
    return true;
}

void printF1 (vector<string> truth_boxes)  {
    vector<Rect> detected_boxes = facemain(("darts/" + truth_boxes.at(0)).c_str());
    Mat frame = imread("darts/"+truth_boxes.at(0), CV_LOAD_IMAGE_COLOR);
///////////////////////////////////////////////////////////////////////////////
    for (int i = 1; i < truth_boxes.size(); i+=4)  {
        int x = stoi(truth_boxes.at(i));
        int y = stoi(truth_boxes.at(i+1));
        int w = stoi(truth_boxes.at(i+2));
        int h = stoi(truth_boxes.at(i+3));
        rectangle(frame, Point(x, y), Point(x+w, y+h), Scalar(255, 255, 0), 2);
    }
    for(int j=0; j < detected_boxes.size(); j++)  {
        rectangle(frame, detected_boxes.at(j), Scalar(0, 255, 0), 2);
    }

    imwrite("subtask2/both" + truth_boxes.at(0), frame);

    ///////////////////////////////////////////////////////////////////////////
    double truePositives = 0;
    double falsePositives = 0;
    double falseNegatives = 0;

    for(int i = 0; i < detected_boxes.size(); i++)  {
        bool matched = false;
        for (int j = 1; (j < truth_boxes.size()) && (matched==false); j+=4)  {
            int xt = stoi(truth_boxes.at(j));
            int yt = stoi(truth_boxes.at(j+1));
            int wt = stoi(truth_boxes.at(j+2));
            int ht = stoi(truth_boxes.at(j+3));
            Rect truth(xt, yt, wt, ht);
            if(compareRect(truth, detected_boxes.at(i)))  {
                truePositives++;
                cout << "Matched " << truth_boxes.at(0) << "\n";
                matched = true;
                truth_boxes.erase(truth_boxes.begin() + j );
                truth_boxes.erase(truth_boxes.begin() + j +1);
                truth_boxes.erase(truth_boxes.begin() + j +2);
                truth_boxes.erase(truth_boxes.begin() + j +3);
            }
        }
    }
    falsePositives = detected_boxes.size() - truePositives;
    falseNegatives = (truth_boxes.size()-1)/4;
    printf("%s :",truth_boxes.at(0).c_str());
    f1score(truePositives, falsePositives, falseNegatives);
}


int main() {
    vector<vector<string> > M = readCSV( "data.csv" );
    for(int i = 0; i< M.size(); i++)  {
        drawrect(M.at(i));
    }
    for(int i = 0; i < M.size(); i++)  {
        printF1(M.at(i));
    }
}
