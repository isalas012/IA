#include <stdio.h>    
#include <stdlib.h>     
#include <time.h>       
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
using namespace std;

/*-------------------------------Data Structures-------------------------------*/

struct problem_data {
    std::vector<int> PS; //Problem Size
    std::vector<vector<int>> CM; //Coverage Matrix
    int CM_size[2]; //Size {rows, cols}
    std::vector<vector<int>> PM; //Preference Matrix
    int PM_size[2]; //Size {rows, cols}
};

struct problem_case {
    int CWS[2]; //Consecutive working shift {min, max}
    int AS[2]; //Assignmetns
    std::vector<vector<int>> CSWS; //Consecutive same working shift
    std::vector<vector<int>> APS; //Assignments per shift
};

struct evaluation_data{
    int EV; //Evaluation score
    std::vector<int> Si; //Score nurse i
    std::vector<int> RBi; // Soft estriction broke by nurse i
    std::vector<int> ASi; // Soft restriction broke in shift i
};

struct element_data{
    std::vector<vector<int>> M; //Evaluation score
    evaluation_data e;
};

struct cola
{
    std::vector<element_data> v;
    int largo = 0;
    int max;
};



/*----------------------------Function Declaration----------------------------*/
problem_data getData(const char* file);
void printMatrix(int rows, int cols, std::vector<vector<int>> M);
problem_case getCase(const char* file);
std::vector<vector<int>> getStartInstance(problem_data d, problem_case c);
evaluation_data getMatrixScore(std::vector<vector<int>> M, problem_data d, problem_case c);
element_data getNeighbors(std::vector<vector<int>> initial_M, problem_data d, problem_case c, cola col);
void insertar(cola &c, element_data elemento);
void tabuSearch(const char* file_data, const char* file_case, int tabuListSize, int iteraciones);
int matrixIsEqual(std::vector<vector<int>> M1, std::vector<vector<int>> M2, problem_data d);
int validMatrix(std::vector<vector<int>> M, problem_data d);
//std::vector<vector<int>> getStartInstanceOld(problem_data d, problem_case c);


/*------------------------------------Main------------------------------------*/

int main(int argc, char const *argv[])
{

    tabuSearch("./NSPLib/N50/1.nsp", "./NSPLib/Cases/1.gen", 50, 50);
    //tabuSearch("./NSPLib/test1/test1.nsp", "./NSPLib/test1/test1.gen", 10, 10);
	return 0;
}

/*---------------------------------Functions---------------------------------*/

void tabuSearch(const char* file_data, const char* file_case, int tabuListSize, int iteraciones){
    cola tabuList;
    tabuList.max = tabuListSize;
    problem_data data;
    problem_case cs;
    element_data best_element;
    element_data actual_element;
    element_data best_neighbor;
    cout << "Cargando datos desde los archivos...\n";
    data = getData(file_data);
    cs = getCase(file_case);
    cout << "Ejecutando tabuSearch" << flush;
    best_element.M = getStartInstance(data, cs);
    best_element.e = getMatrixScore(best_element.M, data, cs);
    insertar(tabuList, best_element);
    //printMatrix(data.CM_size[0],data.CM_size[1], data.CM);
    //printMatrix(data.PM_size[0],data.PM_size[1], data.PM);
    //printMatrix(4,2, cs.CSWS);
    //printMatrix(4,2, cs.APS);
    actual_element = best_element;
    for (int i = 0; i < iteraciones; ++i)
    {
        best_neighbor = getNeighbors(actual_element.M, data, cs, tabuList);
        insertar(tabuList, best_neighbor);
        if (best_element.e.EV < best_neighbor.e.EV)
        {
            best_element = best_neighbor;
        }
        actual_element = best_neighbor;
        cout << "." << flush;
    }
    cout << '\n';
    cout << "La mejor configuracion obtenida con un puntaje de " << best_element.e.EV << " es:" << '\n';
    //printMatrix(data.PM_size[0],data.PM_size[1], best_element.M);
    cout << '\n';
    cout << "Puntaje cada enfermera: " << '\n';
    int total = 0;
    for (int i = 0; i < data.PS[0]; ++i)
    {
        cout << best_element.e.Si[i] << " | ";
        total = total + best_element.e.Si[i];
    }
    cout << '\n';
    cout << "Con un total de: " << total << '\n';
    cout << '\n';
    cout << "Restricciones rotas por cada enfermera: " << '\n';
    for (int i = 0; i < data.PS[0]; ++i)
    {
        cout << best_element.e.RBi[i] << " | ";
    }
    cout << '\n';
    cout << '\n';
    cout << "Restricciones rotas por maximo de enfermeras por turno: " << '\n';
    for (int i = 0; i < data.PM_size[1]; ++i)
    {
        cout << best_element.e.ASi[i] << " | ";
    }
    cout << '\n';
    /*
    for (int i = 0; i < tabuList.largo; ++i)
    {
        printMatrix(data.PM_size[0],data.PM_size[1], tabuList.v[i].M);
    }*/
}


problem_data getData(const char* file){
	ifstream inFile;
	problem_data data;

	std::vector<int> PS(3); // Problem size: [N, D, S]
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
    std::vector<vector<int>> CM(PS[1]); //Coverage Matrix: CM[row][column]
    for (int i = 0; i < PS[1]; ++i)
    {
	    inFile.getline(line, 100);
	    //cout << line << endl;
	    std::stringstream stream2(line);
	    CM[i].resize(PS[2]);
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
    std::vector<vector<int>> PM(PS[0]); //[PS[0]][AS]
    for (int i = 0; i < PS[0]; ++i)
    {
	    inFile.getline(line, 100);
	    //cout << line << endl;
	    std::stringstream stream2(line);
	    PM[i].resize(AS);
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

void printMatrix(int rows, int cols, std::vector<vector<int>> M){
    //cout << "printMatrixFunction----------------------------------------------------------------------------------" << '\n';
	//cout<<"rows: "<< rows <<"\n";
	//cout<<"cols: "<< cols <<"\n";
	for(int x=0;x<rows;x++)  // loop 3 times for three lines
    {
        for(int y=0;y<cols;y++)  // loop for the three elements on the line
        {
            cout<<M[x][y]<<" | " << flush;  // display the current element out of the array
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
    std::stringstream stream0(line);
    //cout << line << endl;
    int temp[2];
    for (int i = 0; i < 2; ++i)
    {
       int n;
       stream0 >> n;
       if(!stream0)
          break;
       //cout << "Found integer: " << n << "\n";
       temp[i] = n;
    }
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
       cs.AS[i] = n;
    }
    inFile.getline(line, 100);
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
       cs.CWS[i] = n;
    }
    inFile.getline(line, 100);
    inFile.getline(line, 100);
    cs.CSWS.resize(temp[1]);
    cs.APS.resize(temp[1]);
    for (int i = 0; i < temp[1]; ++i)
    {
        inFile.getline(line, 100);
        //cout << line << endl;
        std::stringstream stream2(line);
        cs.CSWS[i].resize(2);
        cs.APS[i].resize(2);
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

/*
std::vector<vector<int>> getStartInstanceOld(problem_data d, problem_case c){
    
    std::vector<vector<int>> M(d.PM_size[0]);
    for (int i = 0; i < d.PM_size[0]; ++i)
    { 
        M[i].resize(d.PM_size[1]);
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
}*/

std::vector<vector<int>> getStartInstance(problem_data d, problem_case c){
    std::vector<vector<int>> M;
    int valid = 0;
    double chance = 0.5;
    int it = 0;
    while(!valid){
        M.resize(d.PM_size[0]);
        for (int i = 0; i < d.PM_size[0]; ++i)
            {
                M[i].resize(d.PM_size[1]);
                for (int j = 0; j < d.PM_size[1]; ++j)
                {
                    double n = ((double) rand() / (RAND_MAX));
                    //cout << "Numero random: " << n <<'\n';
                    //cout << "Chance: " << chance <<'\n';
                    if (n <= chance)
                    {
                       M[i][j]= 1;
                    }
                    else{
                       M[i][j]= 0;
                    }
                }
            }
        if (validMatrix(M, d))
        {
            valid = 1;
        }
        else{
            if (chance < 1)
            {
                chance = chance + 0.1;
            }
        }
        it++;
    } 
    //printMatrix(d.PM_size[0], d.PM_size[1], M);
    return M;
}

int validMatrix(std::vector<vector<int>> M, problem_data d){
    int temp = -1;
    for (int i = 0; i <  d.PM_size[1]; ++i)
    {
        if (i%d.PS[2]==0)
        {
            temp++;
        }
        int count = 0;
        for (int j = 0; j < d.PM_size[0]; ++j)
        {
         if (M[j][i]==1)
            {
                count = count + 1;
            }   
        }
        if (d.CM[temp][i%d.PS[2]]>count)
        {
            return 0;
        }
        
    }
    return 1;
}

evaluation_data getMatrixScore(std::vector<vector<int>> M, problem_data d, problem_case c){
    int temp = -1;
    evaluation_data e;
    for (int i = 0; i <  d.PM_size[1]; ++i)
    {
        if (i%d.PS[2]==0)
        {
            temp++;
        }
        int count = 0;
        for (int j = 0; j < d.PM_size[0]; ++j)
        {
         if (M[j][i]==1)
            {
                count = count + 1;
            }   
        }
        if (d.CM[temp][i%d.PS[2]]>count)
        {
            e.EV = -1000000;
            return e;
        }
        
    }    
    e.RBi.resize(d.PM_size[0]);
    int P_s = 0;
    int RN_s = 0;
    int RS_s = 0;
    for (int i = 0; i < d.PM_size[0]; ++i)
    {
        e.RBi[i] = 0;
    }
    e.Si.resize(d.PM_size[0]);
    //Assigments per nurse and Working Shifts per nurse
    for (int i = 0; i < d.PM_size[0]; ++i)
    {
        int nurse_score = 0;
        int max_cs = -1; //maxconsecutvie shift
        int as = 0; //assignments
        int cs = 0; //consecutive shifts
        int maxShiftValues[d.PS[2]];
        int actualShiftValues[d.PS[2]];
        for (int j = 0; j < d.PS[2]; ++j)
        {
            maxShiftValues[j] = 0;
            actualShiftValues[j]=0;
        }
        for (int j = 0; j < d.PM_size[1]; ++j)
        {
            if (M[i][j]==1)
            {   
                cs = cs + 1;
                as = as + 1;
                nurse_score = nurse_score + d.PM[i][j];
                for (int k = 0; k < d.PS[2]; ++k)
                {
                    if (j%d.PS[2]==k)
                    {
                        actualShiftValues[k] = actualShiftValues[k] + 1;
                    }
                }
            }
            if (M[i][j]==0)
            {
                if (cs> max_cs)
                {
                    max_cs = cs;
                }
                cs = 0;
                for (int k = 0; k < d.PS[2]; ++k)
                {
                    if (j%d.PS[2]==k)
                    {
                        if (maxShiftValues[k]<actualShiftValues[k])
                        {
                            maxShiftValues[k] = actualShiftValues[k];
                        }
                    }
                }
                for (int k = 0; k < d.PS[2]; ++k)
                {
                    actualShiftValues[k] = 0;
                }
            }
        }
        if (max_cs < c.CWS[0] || max_cs > c.CWS[1]){
            e.RBi[i] = e.RBi[i] + 1;
        }
        if (as < c.AS[0] || as > c.AS[1])
        {
            e.RBi[i] = e.RBi[i] + 1;
        }
        for (int k = 0; k < d.PS[2]; ++k)
        {
            if (maxShiftValues[k] < c.CSWS[k][0] || maxShiftValues[k] > c.CSWS[k][1])
            {
                e.RBi[i] = e.RBi[i] + 1;
            }
        }
        e.Si[i]=nurse_score;
    }
    for (int i = 0; i < d.PM_size[0]; ++i)
    {
        //cout << e.Si[i] << " | ";
        P_s = P_s + e.Si[i];
    }
    //cout << '\n';
    for (int i = 0; i < d.PM_size[0]; ++i)
    {
        //cout << e.RBi[i] << " | ";
        RN_s = RN_s + e.RBi[i];
    }
    //cout << '\n';

    // Assigmenst per shift
    e.ASi.resize(d.PM_size[1]);
    for (int i = 0; i < d.PM_size[1]; ++i)
    {
        e.ASi[i] = 0;
    }
    for (int i = 0; i <  d.PM_size[1]; ++i)
    {
        int aps = 0;
        for (int j = 0; j < d.PM_size[0]; ++j)
        {
         if (M[j][i]==1)
            {
                aps = aps + 1;
            }   
        }
        if (aps < c.APS[i%d.PS[2]][0] || aps > c.APS[i%d.PS[2]][1])
        {
            e.ASi[i] =  1 ;
        }
        
    }
    for (int i = 0; i < d.PM_size[1]; ++i)
    {
        RS_s =RS_s + e.ASi[i];
    }
    e.EV = P_s - RN_s - RS_s;
    //cout << "Socre i Neighbor: " << e.EV << '\n';
    return e;

}

element_data getNeighbors(std::vector<vector<int>> initial_M, problem_data d, problem_case c, cola col){
    //cout << "getNeighbors\n";
    int temp = 0;
    element_data best_neighbor;
    element_data test_neighbor;
    int firstElement = 1;
    for(int i =0; i<d.PM_size[0]; i++){
        for (int j = 0; j < d.PM_size[1]; ++j)
        {
            if (firstElement)
            {
                //cout << "firstElement----------------------------------\n";
               test_neighbor.M = initial_M;
                if (test_neighbor.M[i][j] == 0)
                {
                    test_neighbor.M[i][j]=1;
                }
                else{
                    test_neighbor.M[i][j]=0;
                }
                test_neighbor.e = getMatrixScore(test_neighbor.M, d, c);
                if (test_neighbor.e.EV==-1000000)
                {
                    continue;
                }
                else{
                    int isTabu = 0;
                    for (int k = 0; k < col.largo; ++k)
                    {
                        if (matrixIsEqual(col.v[k].M, test_neighbor.M, d))
                        {
                            isTabu = 1;
                            //cout << "La matris inicial es tabu\n";
                            break;
                        }
                    }
                    if (!isTabu)
                    {
                        best_neighbor.M = test_neighbor.M, d;
                        best_neighbor.e = getMatrixScore(best_neighbor.M, d, c);
                        firstElement = 0;
                    }
                }
            }
            else{
                test_neighbor.M = initial_M, d;
                if (test_neighbor.M[i][j] == 0)
                {
                    test_neighbor.M[i][j]=1;
                }
                else{
                    test_neighbor.M[i][j]=0;
                }
                test_neighbor.e = getMatrixScore(test_neighbor.M, d, c);
                if (test_neighbor.e.EV==-1000000)
                {
                    continue;
                }
                else{
                    if (test_neighbor.e.EV > best_neighbor.e.EV)
                    {
                        int isTabu = 0;
                        for (int k = 0; k < col.largo; ++k)
                        {
                            if (matrixIsEqual(col.v[k].M, test_neighbor.M, d))
                            {
                                isTabu = 1;
                                //cout << "La matris es tabu\n";
                                break;
                            }
                        }
                        if (!isTabu)
                        {
                            best_neighbor.M = test_neighbor.M, d;
                            best_neighbor.e = getMatrixScore(best_neighbor.M, d, c);
                        }
                    }
                }
            }
            
        }
    }
    //printMatrix(d.PM_size[0], d.PM_size[1], best_neighbor.M);
    //cout << "Best Neigbor Score: " << best_neighbor.e.EV <<'\n';
    if (matrixIsEqual(best_neighbor.M, initial_M, d))
    {
        cout << "WOPS TE QUEDASTE SIN VECINOS\n";
        exit(1);
    }
    return best_neighbor;
}


int matrixIsEqual(std::vector<vector<int>> M1, std::vector<vector<int>> M2, problem_data d){
    for (int i = 0; i < d.PM_size[0]; ++i)
    {
        for (int j = 0; j < d.PM_size[1]; ++j)
        {
            if (M1[i][j] != M2[i][j])
            {
                return 0;
            }
        }
    }
    return 1;
}

void insertar(cola &c, element_data elemento){
    //cout << "Tabu list insert\n";
    //cout << "Elemento score :" << elemento.e.EV << '\n';
    //cout << "Largo :" << c.largo << '\n';

    if (c.largo == c.max)
    {
        //cout << "MAX\n";
        c.v.erase (c.v.begin());
        c.v.push_back (elemento);
    }
    else{
        //cout << "NOT MAX\n";
        c.v.push_back (elemento);
        c.largo = c.largo + 1;
        //cout << "Largo despues:" << c.largo << '\n';
    }
    for (int i = 0; i < c.largo; ++i)
    {
        //cout << c.v[i].e.EV << " | ";
    }
    //cout << '\n';
    //cout << '\n';
}






