#include <stdio.h>
#include <iostream>
#include <stdlib.h>         
#include <time.h>
#include <opencv2/opencv.hpp>   
using namespace cv;
using namespace std;

#define channel		3

/*double activation(const double sigma)
	{
		return 1/(1+exp(-sigma));
	}
*/
Mat Max_pooling(int F_row, int F_col, double ** F,int stride , Mat image)
{
	int A_row=image.rows;
	int A_col=image.cols;
	int M_row = (A_row - F_row)/stride + 1;
    int M_col = (A_col - F_col)/stride+ 1; 
	int i,j,k,l,ch;
	int max = 0;	
	Mat result_Max(M_row,M_col,image.type());


	
	for(ch=0; ch<channel; ch++)
	{
		for(i=0; i < M_row; i++)
    	{
        	for(j=0; j< M_col; j++)
        	{
          		max=0;
				  
		    	for(k=0; k<F_row;k++)
            	{
        			for(l=0; l<F_col; l++)
					{
						if( image.at<cv::Vec3b>(k+i*stride,l+j*stride)[ch] > max ) 
						max = image.at<cv::Vec3b>(k+i*stride,l+j*stride)[ch];
					}
        		}
            result_Max.at<cv::Vec3b>(i,j)[ch] = max;  
			}    	
    	}
	}

	return result_Max;
}

Mat Convolution( int F_row, int F_col, double **F, int padding, int stride, int ***P, Mat image)
{
	clock_t begin,end;
	int A_row=image.rows;
	int A_col=image.cols;
	int ***C;
	int C_row = (A_row - F_row + 2*padding)/stride + 1;
	int C_col = (A_col - F_col + 2*padding)/stride + 1;
	int i,j,k,l,ch;
	
	Mat result_con(C_row,C_col,image.type());

	C = (int ***)malloc(channel*sizeof(int **));    
	for(i=0; i<channel; i++){
		*(C+i) = (int**)malloc(C_row * sizeof(int*));
		for(j=0; j<C_row;j++){
			*(*(C+i)+j) = (int*)malloc(C_col*sizeof(int));
		}
	}
	for(ch=0;ch<channel;ch++)                
	{
		for (i = 0; i < C_row; i++)
		{
			for (j = 0; j < C_col; j++) 
			C[ch][i][j]	= 0;
		}
	}
	begin = clock();
	
	for(ch=0; ch<channel; ch++)
	{
		for(i=0; i< C_row; i++)
    	{
        	for(j=0; j< C_col; j++)
        	{
            	for(k=0; k<F_row;k++)
            	{
                	for(l=0; l<F_col; l++)
                	C[ch][i][j] += P[ch][k+i*stride][l+j*stride] * F[k][l]; 
            	}
			if(C[ch][i][j]>255)
				C[ch][i][j] = 255;
			else if(C[ch][i][j]<0)
				C[ch][i][j] = 0;
			result_con.at<cv::Vec3b>(i,j)[ch] = C[ch][i][j];	 
        	}
    	
    	}
	}
	
    end = clock(); 
    cout<<"총 소요 시간 : "<< (double)(end - begin)/CLOCKS_PER_SEC<<endl;
    
    for(i=0; i<channel;i++) 
	{
		for(j=0; j<C_row; j++)
		{
			free(*(*(C+i)+j));
		}
	free(*(C+i));
	}
	free(C);
	return result_con;
}

int main()
{
	Mat image;
    int i,j,k,l,data;
    int ***A;
    int ***P;
    double **F;
    int stride;
    int padding;
	int ch;
    int F_row;
    int F_col;
    int max=0;
    //////////////////////////////////////////////image
    image = imread("test.jpg", IMREAD_COLOR);

    if(image.empty())
    {
        cout << "Could not open or find the image" << endl;
        return -1;
    } 
	int A_row = image.rows;
    int A_col = image.cols;
    
    
	printf("padding , stride �� �Է�: ");  
	scanf("%d %d",&padding,&stride);
	
	////////////////////////////////image A 3���� �����Ҵ� 
	A = (int ***)malloc(channel*sizeof(int **));
	for(i=0; i<channel; i++){
		*(A+i) = (int**)malloc(A_row * sizeof(int*));
		for(j=0; j<A_row;j++){
			*(*(A+i)+j) = (int*)malloc(A_col*sizeof(int));
		}
	}
	
	for(ch=0; ch<channel; ch++)
	{
		for(i=0; i<A_row; i++)
		{
			for(j=0; j<A_col; j++)
			A[ch][i][j] = image.at<Vec3b>(i,j)[ch];
		}
	}
			
	///////////////////////////////padding
	int P_row = A_row + 2*padding;
	int P_col = A_col + 2*padding;
	
	P = (int ***)malloc(channel*sizeof(int **));
	for(i=0; i<channel; i++){
		*(P+i) = (int**)malloc(P_row * sizeof(int*));
		for(j=0; j<P_row;j++){
			*(*(P+i)+j) = (int*)malloc(P_col*sizeof(int));
		}
	}
	
	for(ch=0;ch<channel; ch++)
	{
		for ( i=0; i < P_row ;i++)
    	{
        	for( j=0; j < P_col ;j++)
        	{
            	if(i<padding || j<padding || i>= A_row+padding || j>= A_col+padding)
            	P[ch][i][j] = 0;
            	else 
            	P[ch][i][j] = image.at<Vec3b>(i-padding,j-padding)[ch];
        	}
    	}
	}
	
	//////////////////////////////////////////////���� F �����Ҵ�
	printf("F_row , F_col �� �Է�: ");  
	scanf("%d %d",&F_row,&F_col); 
    F = (double **)malloc(F_row*sizeof(double *));
	for(i=0; i<F_row; i++)
	{	
		*(F+i)= (double *)malloc(F_col*sizeof(double));

	}
	double x =0;
	for(i=0; i<F_row; i++)
	{
		for(j=0; j<F_col; j++){
			
			scanf("%lf", &F[i][j]);	
			x += F[i][j];
			printf("%lf",F[i][j]);
		}
		printf("\n");
	}
if(x>1)
{
	for(i=0; i<F_row; i++)
	{
		for(j=0; j<F_col; j++){
			
			F[i][j]= F[i][j] / x;
		}	
	}
}
	Mat result_max = Max_pooling(F_row, F_col,F, stride,image);
		
	namedWindow("Max", WINDOW_AUTOSIZE);
	imshow("Max",result_max);
	//imwrite("Max.jpg",result_max);
	//waitKey(0);

	Mat result_con = Convolution(F_row, F_col, F, padding, stride, P, image);
	namedWindow("Con", WINDOW_AUTOSIZE);
	imshow("Con",result_con);
	//imwrite("Con.jpg",result_con);
	waitKey(0);
	
}
    
    
    
    
