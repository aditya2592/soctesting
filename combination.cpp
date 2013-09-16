# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <time.h>

#define MAX 100

void comb(int m , int n)
{
	int arr[MAX], dumm[MAX], flag[MAX];
	int temp_index = 0;
		
	for(int i = 0; i< n; i++)
		flag[i] = 0;
	for(int i = 0; i< n; i++)
		arr[i] = i+1;
	for(int i = 0; i< m; i++)
		dumm[i] = i+1;	
	while(arr[0] <= dumm[m- n])
	{
		for (int j = 0; j< n; j++)
			printf("%d\t", arr[j]);
		printf("\n");
		
		while(arr[n-1] < dumm[m-1])
		{	
			arr[n-1] = arr[n-1] + 1;
			for (int j = 0; j< n; j++)
				printf("%d\t", arr[j]);
			printf("\n");
		}
	
		for(int i = 1 ; i < n; i++)
		{
			if (arr[(n-1) - i + 1] == dumm [(m-1) - i + 1])
				flag[(n-1) - i] = 1;
		}
		
		for (int k = 0; k < n; k++)
		{
			if (flag [k] == 1)
			{	
				temp_index = k;
				break;
			}
		}
		arr[temp_index] = arr [temp_index] + 1;
		for (int l = temp_index; l< n ;l++)
			arr[l+1] = arr[l] + 1;
				
		for (int j = 0; j< n; j++)
			flag [j] = 0;		
	}		
}


int main (int argc, char *argv[])
{
	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	comb(m,n);
}
