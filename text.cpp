#include <iostream>
#include <fstream>
using namespace std;
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <chrono> 
using namespace std::chrono;

int row1=0;
int col1=0;
int row22=0;
int col22 =0;



typedef struct ThreadArgs {
    int* arr_row;
    int* arr_col;
}arguments;


typedef struct ThreadSecond {
    int* arr_row2;
    int** arr_cols2;
}second;





void* multiplication(void* args){
//cout << "here";
ThreadArgs* arg = static_cast<ThreadArgs*>(args);

//int* arr_row = arg->arr_row;
    //int* arr_col = arg->arr_col;
    int sum = 0;
    
    /*cout<<"--------------";
    for(int i=0 ;i<col1;i++)
    cout<<arg->arr_row[i]<<" rr";
    for(int i=0 ;i<col22;i++)
     cout<<arg->arr_col[i]<<" cc";
	cout<<"--------------";*/
      int i=0,j =0;
    while (i < col1){
    sum += arg->arr_row[i] * arg->arr_col[j];
    i+=1;
    j+=1;
    }
    //cout <<"sum is" << sum;
    pthread_exit((void*)sum);
    return NULL;
}



void* Second_multiplication(void* args) {
    ThreadSecond* arg = static_cast<ThreadSecond*>(args);

   int* sum = new int[col22]();

    for (int k = 0; k < col22; ++k) {
        int result = 0;
        for (int r = 0; r < row22; r++) {
            result += arg->arr_row2[r] * arg->arr_cols2[r][k];
        }
        sum[k] = result;
        //cout<<"\n"<<sum[k]<<"  ";
    }
	//cout<<"\n end of fun \n";
    pthread_exit((void *)sum);
    return NULL;
}


int main(int argv,char ** argc) {
    int row=0,col=0,col2=0,row2=0;
    //cout<<"enter name of the file:\n";
    string name;
    cout<<"\n";
    ifstream inputFile1(argc[1]);  
    if (!inputFile1.is_open()) {
        cout << "File opening failed!" << endl;
      return 1;
    }

    //read first matrix
    inputFile1 >> row >> col;
    int** matrix_mul1 = new int*[row];
    for (int i = 0; i < row; i++) {
        matrix_mul1[i] = new int[col];
        for (int j = 0; j < col; j++) {
            inputFile1 >> matrix_mul1[i][j];
        }
    }

    row1 = row;
    col1 = col;
    
    /*	    for(int i =0 ;i<row;i++){
    cout<<"\n";
    	for(int j =0 ;j<col;j++){
    		cout << matrix_mul1[i][j]<<" ";
    		}
    }*/

    //read second matrix
    inputFile1 >> row2 >> col2;
    int** matrix_mul2 = new int*[row2];
    for (int i = 0; i < row2; i++) {
        matrix_mul2[i] = new int[col2];
        for (int j = 0; j < col2; j++) {
            inputFile1 >> matrix_mul2[i][j];
        }
    }
     row22= row2;
    col22 = col2;
    cout<<"\n";
    cout<<"\n";

     /*    for(int i =0 ;i<row2;i++){
    cout<<"\n";
    	for(int j =0 ;j<col2;j++){
    		cout << matrix_mul2[i][j] <<" ";
    		}
    }*/
    if(col != row2){
    cout<<"Multipication of Matrix is not Possible !!\n";
    exit(0);
    }

    int** output_matrix = new int*[row];
    for (int i = 0; i < row; i++) {
        output_matrix[i] = new int[col2]();
    }

     //first approach 
     
    auto start = high_resolution_clock::now();
    pthread_t* threads = new pthread_t[row * col2];
    int count = 0;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col2; j++) {
            arguments* arg = new arguments;
            arg->arr_row = new int[col]; //store row
            arg->arr_col = new int[row2];//store column
	    
	    for(int k =0;k < col;k++){
        	arg->arr_row[k] =  matrix_mul1[i][k]; //row
        	
        }
        
            for (int k = 0; k < row2; k++) {
                arg->arr_col[k] = matrix_mul2[k][j];   //column
            }

            pthread_create(&threads[count++], NULL, multiplication, (void*)(arg));
        }
    }

    //wait for threads untill finish
       int j =0;
       int k =0;
       int sum;
    for (int i = 0; i < row * col2; ++i) {
    	sum=0;
        pthread_join(threads[i], (void **)&sum); 
        //cout<< sum <<"\n";	
	output_matrix[k][j] = sum;
	j++;
	if(j == col2){
		j =0;
		k++;
		}
    }

      cout<<"By Element";
    //print output for first approach
     for(int i =0 ;i<row;i++){
    cout<<"\n";
    	for(int j =0 ;j<col2;j++){
    		cout << output_matrix[i][j]<<" ";
    	}
    	}
    	
    auto stop = high_resolution_clock::now();
    	auto duration = duration_cast<microseconds>(stop - start);		
    	cout << "\nTime : " << duration.count() / 1000000.0 << " s" << endl;

    //second approach
    //use the same input arrays
    

     //second approach 
    auto start2 = high_resolution_clock::now();
    pthread_t* threads_second = new pthread_t[row];
	//cout<<row<<" row1";
    count = 0;
 for (int i = 0; i < row; i++) {
       
    	second* arg = new second;
        arg->arr_cols2 = matrix_mul2; //contain all columns(second array) 
       arg->arr_row2 = new int[col]; //store row
       for(int k =0;k < col;k++){
        	arg->arr_row2[k] =  matrix_mul1[i][k]; //row
        	
        }

        pthread_create(&threads_second[count++], NULL, Second_multiplication, (void*)(arg));
    }
    

       for (int i = 0; i < row; ++i) {
    	int *sum;
        pthread_join(threads_second[i], (void **)&sum); 
       
        for(int j = 0 ;j<col2;j++){
		 output_matrix[i][j] = sum[j];      
        }

        delete[] sum;
    }

    cout<< "\nBy Row" << endl;
    //print output for second approach
    for(int i =0 ;i<row;i++){
    cout<<"\n";
    	for(int j =0 ;j<col2;j++){
    		cout << output_matrix[i][j]<<" ";
    		}
    	}
    	
    auto stop2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(stop2 - start2);	
    	cout << "\nTime : " << duration2.count() / 1000000.0 << " s" << endl;
    return 0;
}

