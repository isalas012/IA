#include <stdio.h>    
#include <stdlib.h>     
#include <time.h>       
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

/*-------------------------------Data Structures-------------------------------*/

struct problem_data {
    int* PS; //Problem Size
    int** CM; //Coverage Matrix
    int CM_size[2]; //Size {rows, cols}
    int** PM; //Preference Matrix
    int PM_size[2]; //Size {rows, cols}
};

struct problem_case {
    int CWS[2]; //Consecutive working shift {min, max}
    int AS[2]; //Assignmetns
    int** CSWS; //Consecutive same working shift
    int** APS; //Assignments per shift
};

/*----------------------------Function Declaration----------------------------*/
problem_data getData(const char* file);
void printMatrix(int rows, int cols, int** M);
problem_case getCase(const char* file);
int** getStartInstance(problem_data d, problem_case c);


/*------------------------------------Main------------------------------------*/

int main(int argc, char const *argv[])
{
	problem_data data;
    problem_case cs;
    int ** start;

	data = getData("./NSPLib/N25/1.nsp");
    cs = getCase("./NSPLib/Cases/1.gen");
	//cout << data.PS[0];
	printMatrix(data.CM_size[0],data.CM_size[1], data.CM);
	printMatrix(data.PM_size[0],data.PM_size[1], data.PM);
    printMatrix(4,2, cs.CSWS);
    printMatrix(4,2, cs.APS);
    start = getStartInstance(data, cs);
    printMatrix(data.PM_size[0],data.PM_size[1], start);
	return 0;
}

/*---------------------------------Functions---------------------------------*/

problem_data getData(const char* file){
	ifstream inFile;
	problem_data data;

	int* PS; // Problem size: [N, D, S]
	PS = new int[3];

    inFile.open(file);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    
    char line[100];
    inFile.getline(line, 100);
    std::stringstream stream(line);
    //cout << line << endl;
    for (int i = 0; i < 3; ++i)
    {
	   int n;
	   stream >> n;
	   if(!stream)
	      break;
	   //cout << "Found integer: " << n << "\n";
	   PS[i] = n;
    }
    inFile.getline(line, 100);
    //cout << line << endl;
    int** CM; //Coverage Matrix: CM[row][column]
    CM = new int*[PS[1]];
    for (int i = 0; i < PS[1]; ++i)
    {
	    inFile.getline(line, 100);
	    //cout << line << endl;
	    std::stringstream stream2(line);
	    CM[i] = new int[PS[2]];
	    int j=0;
	    while (1)
	    {
		   int n;
		   stream2 >> n;
		   if(!stream2)
		      break;
		   //cout << "Found integer: " << n << "\n";
		   CM[i][j] = n;
		   j++;
	    }
    }
    inFile.getline(line, 100);
    //cout << line << endl;
    int AS = PS[1]*PS[2]; //all shifts
    int** PM; //[PS[0]][AS]
    PM = new int*[PS[0]];
    for (int i = 0; i < PS[0]; ++i)
    {
	    inFile.getline(line, 100);
	    //cout << line << endl;
	    std::stringstream stream2(line);
	    PM[i] = new int[AS];
	    int j=0;
	    while (1)
	    {
		   int n;
		   stream2 >> n;
		   if(!stream2)
		      break;
		   //cout << "Found integer: " << n << "\n";
		   PM[i][j] = n;
		   j++;
	    }
    }
    inFile.close();
    data.PS = PS;
    data.CM = CM;
    data.CM_size[0] = PS[1];
    data.CM_size[1] = PS[2];
    data.PM = PM;
    data.PM_size[0] = PS[0];
    data.PM_size[1] = AS;
    return data;
}

void printMatrix(int rows, int cols, int** M){
        cout << "printMatrixFunction----------------------------------------------------------------------------------" << '\n';
	cout<<"rows: "<< rows <<"\n";
	cout<<"cols: "<< cols <<"\n";
	for(int x=0;x<rows;x++)  // loop 3 times for three lines
    {
        for(int y=0;y<cols;y++)  // loop for the three elements on the line
        {
            cout<<M[x][y]<<" | ";  // display the current element out of the array
        }
    	cout<<endl;  // when the inner loop is done, go to a new line
    }
}

problem_case getCase(const char* file){
    ifstream inFile;
    problem_case cs;

    inFile.open(file);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    char line[100];
    inFile.getline(line, 100);
    inFile.getline(line, 100);
    inFile.getline(line, 100);
    std::stringstream stream1(line);
    //cout << line << endl;
    for (int i = 0; i < 2; ++i)
    {
       int n;
       stream1 >> n;
       if(!stream1)
          break;
       //cout << "Found integer: " << n << "\n";
       cs.CWS[i] = n;
    }
    inFile.getline(line, 100);
    std::stringstream stream2(line);
    //cout << line << endl;
    for (int i = 0; i < 2; ++i)
    {
       int n;
       stream2 >> n;
       if(!stream2)
          break;
       //cout << "Found integer: " << n << "\n";
       cs.AS[i] = n;
    }
    inFile.getline(line, 100);
    inFile.getline(line, 100);
    inFile.getline(line, 100);
    cs.CSWS = new int*[2];
    cs.APS = new int*[2];
    for (int i = 0; i < 4; ++i)
    {
        inFile.getline(line, 100);
        //cout << line << endl;
        std::stringstream stream2(line);
        cs.CSWS[i] = new int[2];
        cs.APS[i] = new int[2];
        int j=0;
        while (1)
        {
           int n;
           stream2 >> n;
           if(!stream2)
              break;
           //cout << "Found integer: " << n << "\n";
           if (j<2)
           {
                cs.CSWS[i][j] = n;
           }
           else{
                cs.APS[i][j-2] = n;
           }
           j++;
        }
    }
    return cs;
}

int** getStartInstance(problem_data d, problem_case c){
    int ** M;
    M = new int*[d.PM_size[0]];
    for (int i = 0; i < d.PM_size[0]; ++i)
    { 
        M[i] = new int[d.PM_size[1]];
        for (int j = 0; j < d.PM_size[1]; ++j){
            M[i][j] = 0;
        }
    }
    //printMatrix(d.PM_size[0], d.PM_size[1], M);
    srand (time(NULL));
    int temp = -1;
    for (int i = 0; i < d.PM_size[1]; ++i)
    {   
        if (i%4 == 0)
        {
            temp++;
        }
        int randomArray[d.CM[temp][i%4]];
        for (int j = 0; j < d.CM[temp][i%4]; ++j)
        {       
            while(1){
                int ok = 1;
                int n;
                n = rand()% d.PM_size[0];
                for(int k = 0; k < j; k++){
                     if(randomArray[k] == n){
                         ok = 0;
                     }
                }
                if (ok)
                {
                    randomArray[j] = n;
                    break;
                }                
            }
        }
        for (int j = 0; j < d.CM[temp][i%4]; ++j)
        {
            //cout << randomArray[j] << "\n";
            M[randomArray[j]][i] = 1;
            
        }
        //cout << '\n';
        //cout << "------------------------------------" << '\n';
    }
    //printMatrix(d.PM_size[0], d.PM_size[1], M);
    return M;
}




