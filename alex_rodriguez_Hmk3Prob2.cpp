/*
Author: Alex Rodriguez
Class: ECE4122  
Last Date Modified: 10/6/2019
Description: A multithreaded program that takes in a command line arguement as the data file and 
computes the largest product of four adjacent numbers in the same direction in a grid.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <omp.h>

using namespace std;

vector <vector<int> > grid; 
//#define CHUNKSIZE 100
int numRows;
int numCols;


int main(int argc, char* argv[] )
{
	//const int iNUM_THREADS = 4;
	//int chunk;
	int i,j;

	if(argv[1])
	{
		double start_time = omp_get_wtime(); // starts timer
		string filename = argv[1];
		string val;

		int rowIndex = 0;
		int colIndex = 0;
		int currentCell;
		int largestProd;
		ifstream myFile (filename.c_str()); // reads file
		if(myFile.is_open())
		{
			if(getline(myFile, val))
			{
				stringstream stream_val(val); // gets each number in the current line one at a time
				stream_val >> numRows; // first row in text file, 1st element 
				stream_val >> numCols; // first row in text file, 2nd element
				//cout << "Number of rows: " << numRows << endl;
				//cout << "Number of cols: " << numCols << endl;
				grid.resize(numRows); // resizes the number of rows
				#pragma omp parallel num_threads(1)
				{
					#pragma omp for schedule(static)
						for(int i = 0; i < numRows; i++)
					{
						grid[i].resize(numCols); // resizes the number of columns in each row
					}

				}


				while(getline(myFile,val))
				{

					stringstream stream_val(val);
					colIndex = 0; 
					while(1) // gets each number in each column until there is no valid column(stream_val) left, then moves to next row(line)
					{
						stream_val  >> grid[rowIndex][colIndex];
						if(!stream_val)
						{
							break;
						}
						colIndex++;
					}
					rowIndex++;


				}

			}
			myFile.close();	
		}
		// largest product of the grid of 4 elements in same direction
		largestProd = 0;

		// sweeps through the matrix from left to right starting at top left of matrix. This allows to only have to check for the next 3 to the right,
		// the next three down, the next three diagonally right, and the next three diagonally left.
		
		//omp_set_dynamic(1);
		#pragma omp parallel num_threads(4) shared(grid,largestProd) private(i,j,currentCell) // creates 4 threads with shared grid and product
		{
			//chunk = CHUNKSIZE;
			omp_set_nested(1);
			#pragma omp for schedule(static) nowait collapse(2) // omp for loop that uses static scheduling and default chunksize to allocate iterations to each thread
				for(int i = 0; i < numRows; i++)
				{
					for(int j = 0; j < numCols; j++)
					{
						
						//#pragma omp parallel //num_threads(1) //private(currentCell)
						{
							//#pragma omp sections nowait
							{
								//#pragma omp section 
								{
									//check next 3 elements to the right
									if(j + 3 < numCols)
									{
										currentCell =  grid[i][j] * grid[i][j+1] * grid[i][j+2] * grid[i][j+3];
										if(largestProd < currentCell)
										{
											largestProd = currentCell;
										}
									}

								}
					
								//#pragma omp section 
								{
									//check next 3 elements down
									if(i + 3 < numRows)
									{
										currentCell = grid[i][j] * grid[i+1][j] * grid[i+2][j] * grid[i+3][j];
										if(largestProd < currentCell)
										{
											largestProd = currentCell;
										}
									}
								}
								
								//#pragma omp section
								{
									//check next 3 elements diagonally down to the right 3 
									if(i + 3 < numRows && j + 3 < numCols)
									{
										currentCell = grid[i][j] * grid[i+1][j+1] * grid[i+2][j+2] * grid[i+3][j+3];
										if(largestProd < currentCell)
										{
											largestProd = currentCell;
										}
									}
									
								}
								
								//#pragma omp section 
								{
									//checks next 3 elements diagonally down to the left
									if(j >= 3 && i + 3 < numRows)
									{
										currentCell = grid[i][j] * grid[i+1][j-1] * grid[i+2][j-2] * grid[i+3][j-3];
										if(largestProd < currentCell)
										{
											largestProd = currentCell;
										}
									}
									
								}
							}
						}
					}
				}
				

		}


		ofstream outputFile ("output2.txt"); // writes out the largest product to the output file
		if (outputFile.is_open())
		{
			outputFile << largestProd;
			outputFile.close();
		}
	//cout << "Time elapsed with openmp: " << (omp_get_wtime() - start_time) << "seconds" << endl;

	}
	else
	{
		cout << "Include the text file as command line args!" << endl;
	}


}