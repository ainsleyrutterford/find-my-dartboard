#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

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
   write( M );
}
