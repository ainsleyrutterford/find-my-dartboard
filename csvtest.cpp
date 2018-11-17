#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv/cv.hpp>
using namespace std;
using namespace cv;

//==========================================================


vector<vector<string> > readCSV( string filename ) {
   vector<vector<string> > M;

   ifstream in( filename );
   string line;
   while ( getline( in, line ) )                   // read a whole line of the file
   {
      stringstream ss( line );                     // put it in a stringstream (internal stream)
      vector<string> row;
      string data;
      while ( getline( ss, data, ',' ) )           // read (string) items up to a comma
      {
         row.push_back( data );            // use stod() to convert to double; put in row vector
      }
      if ( row.size() > 0 ) M.push_back( row );    // add non-empty rows to matrix
   }
   return M;
}


//==========================================================


void write( const vector<vector<string> > &M )
{
   const int w = 12;
// cout << fixed;

   for (int i = 0; i < M.size(); i++) {
     for (int j = 0; j < M.at(i).size(); j++) cout << setw( w ) << M.at(i).at(j) << ' ';
      cout << '\n';
   }
}


//==========================================================


int main()
{
   vector<vector<string> > M = readCSV( "data.csv" );
   Mat frame = imread(M.at(0).at(0), CV_LOAD_IMAGE_COLOR);
   int x = stoi(M.at(0).at(1));
   int y = stoi(M.at(0).at(2));
   int w = stoi(M.at(0).at(3));
   int h = stoi(M.at(0).at(4));

   rectangle(frame, Point(x, y), Point(x+w, y+h), Scalar(255, 255, 0), 2);
   imwrite("testrect.jpg", frame);
   write( M );
}
