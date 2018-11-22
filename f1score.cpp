// header inclusion
// header inclusion

#include <stdio.h>
using namespace std;

double f1score(double truePositives, double falsePostives, double falseNegatives);

// int main(int n, char ** args)  {
//     double truePositives  = (double)atoi(args[1]);
//     double falsePostives  = (double)atoi(args[2]);
//     double falseNegatives = (double)atoi(args[3]);
//     f1score(truePositives, falsePostives, falseNegatives);
//     return 0;
// }

double f1score (double truePositives, double falsePostives, double falseNegatives)  {
    printf("TP: %f, FP: %f, FN: %f\n", truePositives, falsePostives, falseNegatives);
    double precision = truePositives/(truePositives+falsePostives);
    double recall = truePositives/(truePositives+falseNegatives);
    if( truePositives ==  0)  {
        printf("0\n");
        return 0;
    }
        
    double f1 =  2*precision*recall/(precision+recall);
    printf("%f\n", f1);
    return f1;
}
