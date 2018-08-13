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

/*----------------------------Function Declaration----------------------------*/
problem_data getData(const char* file);
void printMatrix(int rows, int cols, int** M);


/*------------------------------------Main------------------------------------*/

int main(int argc, char const *argv[])
{
	problem_data data;
	data = getData("./NSPLib/N25/1.nsp");
	//cout << data.PS[0];
	printMatrix(data.CM_size[0],data.CM_size[1], data.CM);
	printMatrix(data.PM_size[0],data.PM_size[1], data.PM);
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
    cout << line << endl;
    for (int i = 0; i < 3; ++i)
    {
	   int n;
	   stream >> n;
	   if(!stream)
	      break;
	   cout << "Found integer: " << n << "\n";
	   PS[i] = n;
    }
    inFile.getline(line, 100);
    cout << line << endl;
    cout << PS[1] << endl;
    int** CM; //Coverage Matrix: CM[row][column]
    CM = new int*[PS[1]];
    for (int i = 0; i < PS[1]; ++i)
    {
    	cout << "i: " << i << endl;
	    inFile.getline(line, 100);
	    cout << line << endl;
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
    cout << line << endl;
    int AS = PS[1]*PS[2]; //all shifts
    int** PM; //[PS[0]][AS]
    PM = new int*[PS[0]];
    for (int i = 0; i < PS[0]; ++i)
    {
    	cout << "i: " << i << endl;
	    inFile.getline(line, 100);
	    cout << line << endl;
	    std::stringstream stream2(line);
	    PM[i] = new int[AS];
	    int j=0;
	    while (1)
	    {
		   int n;
		   stream2 >> n;
		   if(!stream2)
		      break;
		   cout << "Found integer: " << n << "\n";
		   PM[i][j] = n;
		   j++;
	    }
    }

	for(int x=0;x<PS[0];x++)  // loop 3 times for three lines
    {
        for(int y=0;y<AS;y++)  // loop for the three elements on the line
        {
            cout<<PM[x][y]<<" | ";  // display the current element out of the array
        }
    	cout<<endl;  // when the inner loop is done, go to a new line
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


