#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;


int main(int argc, char const *argv[])
{
	ifstream inFile;

	int l1[3]; // [N, D, S]

    inFile.open("./NSPLib/N25/1.nsp");
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
	   l1[i] = n;
    }
    inFile.getline(line, 100);
    cout << line << endl;
    cout << l1[1] << endl;
    for (int i = 0; i < l1[1]; ++i)
    {
    	cout << "i: " << i << endl;
	    inFile.getline(line, 100);
	    cout << line << endl;
	    std::stringstream stream2(line);
	    int j=0;
	    while (1)
	    {
		   int n;
		   stream2 >> n;
		   if(!stream2)
		      break;
		   cout << "Found integer: " << n << "\n";
		   j++;
	    }
    }
    inFile.getline(line, 100);
    cout << line << endl;
    inFile.close();
	return 0;
}