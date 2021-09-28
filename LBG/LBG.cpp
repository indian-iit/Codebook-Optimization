// LBG.cpp : Defines the entry point for the console application.
//The below programe implements LBG algorithm to optimize the codebook.
//Including required standard libraries
//time.h library is used in generating random numbers and math.h is used to perform mathemathical operations.
#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <float.h>
#include <ctype.h>
//Including required macro definition so that they can be used in below programe.
//MAX finds the maximum number among two numbers.
//Similary MIN finds the min among given two numbers.
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
//Value of P(12) indicates no of values in a single vector(row) in codebook.
//Value of k(8) indicates no of rows(vectors) in our codebook.
//Value of epsilon is used in LBG.
#define epsilon 0.03
#define p 12
#define k 8
//universe_size indicates no of vectors in "universe.csv" file.
//Delta value is used in verifying threshold condition in k-means algorithm.
//row_widht indicates no of digits that can be in a single row.
#define universe_size 6340
#define delta 0.00001
#define row_widht 1000
//Declaring and initialising required global variables so that they can be used acroos any function in the prgrame.
//Tokhura_weights helps us to find distance between 2 vectors.
//universe_array will hold the all the vectors present in the universe file.
//Where as codebook array hols the 8 vectors present in the codebook.
double tokhura_weights[]={1.0,3.0,7.0,13.0,19.0,22.0,25.0,33.0,42.0,50.0,56.0,61.0};
double universe_array[universe_size][p]={0};
double codebook[k][p]={0};
int current_codebook_size=1;
//current_codebook_size indicates size of codebook currently in that particular iteration.
double temp_codebook[k][p]={0};
double current_distort=0;
//Stroring file name of our universe file in an char array.
char universe_file[100]="universe.csv";
//pointer_to_universe_file will help us to point to the unviverse file.
FILE *pointer_to_universe_file;

//The below function will perform k-means Algorithm.
void K_means_Algo()
{
	//Declaring and initialising required variables.
	//nearest_index will store the vector number of the nearest vector in the codebook.
	int nearest_index=0;
	//nearest_index_distance will store the distance between current vector and nearest_index vector.
	double nearest_index_distance=DBL_MAX;
	double previous_distort,total_distort;
	//current_distort will hold the distortion value occured i current iteration.
	//Where as previous_distort will hold the distortion occured in previous iteration.
	double bucket_sum[k][p]={0.0};
	double  tokhura_dist[k] = {0.0};
	//tokhura_dist will say distance between 2 vectors.
	int vector_count[k]={0};
	//vector_count will tell no of vectors in each bucket of the codebook.
	previous_distort=total_distort=current_distort=0;
	do 
	{
		//No we are performing classification step.
	    //We will assign  every vector in universe to some vector in codebook.
		for(int i=0;i<universe_size;i++)
		{
		   //Re_initialising  nearest_index_distance so that previous iteration value will be erased.
		   nearest_index_distance=DBL_MAX;
		   //Running through all vectors in codebook.
		   for (int j = 0; j < current_codebook_size; j++) 
		   {
			  //Re_initialising  tokhura distance so that previous iteration value will be erased.
			  tokhura_dist[j] = 0.0;
			  //Cummulatively Computing tokhura distance for all 12 values in a vector. 
			  for (int k1 = 0; k1 < p; k1++) 
			  {
				tokhura_dist[j] += tokhura_weights[k1]*(universe_array[i][k1]-codebook[j][k1])*(universe_array[i][k1]-codebook[j][k1]); //find out the distance
			  }
		   }
		   //Finding out nerest vector in codebook for the current codebook.
           for (int y = 0; y < current_codebook_size; y++)
		   {
			 //Updating nearest_index_distance and also nearest vector.
			  nearest_index_distance=MIN(nearest_index_distance,tokhura_dist[y]);
			  if (nearest_index_distance == tokhura_dist[y])
			  {
				nearest_index=y;
			  }
		   }
		   //Updating vector count.
		   vector_count[nearest_index]++;
		   //Updating our bucket sum so that it will be useful in finding centroids.
		   for (int y = 0; y < p; y++)
	       {
			 bucket_sum[nearest_index][y] += universe_array[i][y];
	       }
		   //Updating total distortion.
		   total_distort =total_distort+nearest_index_distance;
	 }
	 //Here we are updating our codebook for our next iteration by finding centroid of all buckets.
	 //Running through all 8 vectors in codebook.
	 for (int a = 0; a < current_codebook_size; a++)
	 {
		//Running through all 12 values present in every vector.
		for (int b = 0; b < p; b++)
		{
			codebook[a][b] = bucket_sum[a][b]/vector_count[a];
			//Re_initialising bucket_sum so that they can be used in next iteration by erasing values obtained in current iteration.
			bucket_sum[a][b]=0;
		}
		//Re_initialising vector_count so that they can be used in next iteration by erasing values obtained in current iteration.
		vector_count[a] = 0;
	 }
	 //Updating our distortion values.
	 previous_distort = current_distort; 
	 current_distort = total_distort / universe_size; 
	 //Re-initialising the total_distort value so that it can used in next iteration by erasing value obtained in current iterationn.
	 total_distort=0;
	 }while (abs(previous_distort-current_distort)>delta);
	 //The above condition is used as an threshold condition, wich is used to decide whether to proceed to next iteration or not.
}

//The below function will perform LBG Algorithm to obtain optimal codebook.
void LBG_Algo()
{
	//We will initialise our codebook with size 1.
	//We keep on doubling the size of codebook until we reach size of 8.
	//Here value of k is 8.
	while(current_codebook_size!=k)
	{
		//Running through all vectors present in codebook.
	    for (int a = 0; a < current_codebook_size; a++)
		{
			//Running through all 12 values present ineach vector.
			//Updating tempcodebook.
			//tempcodebook acts as an temparary codebook.
			for (int b = 0; b < p; b++)
			{
				temp_codebook[2*a][b] = codebook[a][b]*(1+epsilon);
				temp_codebook[(2*a)+1][b] = codebook[a][b]*(1-epsilon);
			}
		}
		//Copying values stored in tempcodebook toour original codebook. 
		//Running through all vectors present in the codebook.
		for(int a=0;a<k;a++)
		{
			//Running through all 12 values present in the codebook.
			for(int b=0;b<p;b++)
			{
				codebook[a][b]=temp_codebook[a][b];
			}
		}
		//Updating and doubling the size of the codebook.
		current_codebook_size=current_codebook_size*2;
		printf("\nProcessing :-) current code book size is = %d\n",current_codebook_size);
		//Calling k_means() algorithm to identify centroids for our next iteration to obtain optimal result.
		K_means_Algo();
    }
	printf("\n*************************************************************************************************************************************************************************\n");
	printf("\nPrinting the Final codebook generated after performimg LBG algorithm \n");
	//Printing the final optimised codebook.
	//Loopingthrough all 8 vectors.
	for (int a = 0; a < k; a++) 
	{
		//Looping through all 12 values present in a vector.
		for (int b = 0; b < p; b++)
		{
			printf("%lf   ",codebook[a][b]); 
		}
		printf("\n");
	}
	printf("\n*************************************************************************************************************************************************************************\n");
	printf("\nDistortion obtained in last iteration of LBG Algo is  = %lf\n",current_distort);
}

//The below function will copy all vectors in the universe file to universe_array array.
void copy_universe_file()
{
	//Opening universe.csv file in "read" mode.
	pointer_to_universe_file=fopen("universe.csv","r");
	//Array read_row is used to store single row in universe file, which is usedas an temporary array.
	char read_row[row_widht];
	char	*divide_row;
	//row_index indicates currently in which row of universe file we are present.
	//Similary column_index indicates currently in which column of universe_file we are present.
	int row_index,column_index;
	//Initialising required variables.
	row_index=column_index=0;
	//fgets reads a single row from universe file into read_row.
	//We will continue until there are no more rows left.
    while(fgets(read_row,row_widht,pointer_to_universe_file)!=NULL)
    {
		//Re_initialising column_index so that any value stored in previous iterations will be erased.
		column_index=0;
		//We are dividing the row by comma, to indicate differet columns.
		//Here we are using comma as an deliminating charecter.
        divide_row = strtok(read_row,",");
		//Until no more columns left.
		while(divide_row!=NULL)
		{
			//atof will convert char data type to float data type.
			universe_array[row_index][column_index] = atof(divide_row);
			//We are going on the next column and also updating the column number.
			divide_row = strtok(NULL,",");
			column_index=column_index+1;
        }
		//Updating the row number.
		row_index=row_index+1;
	}
	//Uncomment below code to check whether values in universe file are copied into array properly or not.
	//Like in below commented code i am checking whether 1st row(0th index) of array.
	/*
	for(int i=0;i<p;i++)
		printf("%lf  ",universe_array[6339][i]);
    */
}

//The below function will compute the centroid all vectors in the universe file and store it in codebook.
void initialise_codebook()
{
	//Computing centroid of universe.
	//Running over all vectors in the universe.
	for (int a = 0; a < universe_size; a++)
	{
		//Running over all 12 values and adding up the values present in them.
		for (int b = 0; b < p; b++)
		{
			codebook[0][b] += universe_array[a][b];
		}
	}
	//Printing and computing centroid.
    printf("\nPrinting Iinitial codebook which is centroid of the universe\n"); 
	//Dividing by universe size to determine the average.
	for (int y = 0; y < p; y++)
	{
	    codebook[0][y] =codebook[0][y]/universe_size;
		printf("%lf  |",codebook[0][y]);
    }
	printf("\n");
}

//main() functions indicates the entry point of the programe.
int main()
{
	//copy_universe_file() will copy all the vectors present in the universe.csv to an array.
	copy_universe_file();
	//Once we have copied vectors present in universe file to array,now we will initialise the codebook.
	initialise_codebook();
	//Running LBG algorithm.
	LBG_Algo();
	//Closing all the files opened in the process of generating optimal codebook.
	fclose(pointer_to_universe_file);
	//getch() is used to display the output.
	getch();
}