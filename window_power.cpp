#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using std::ifstream;
#include <cstring>
using namespace std;
#include "soctest.h"
#include <sstream>
#include <algorithm>

string dirsoc = string("/media/aditya25/E23CD82F3CD7FD0B/BTP/SOC_testing/");
//char power_dir[100] = "full/power_value_";
string powerdir = string("/media/aditya25/E23CD82F3CD7FD0B/BTP/SOC_testing/");
const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 3;
const char* const DELIMITER = "	";
const int MAX_SPACE_DELIM = 10;
int MAX_CORES = 50;
int MAX_TAM_WIDTH = 16;
const int MAX_TAM_NUMBER = 100;  //Used for array size constants
int MAX_TAM_NUMBER_IP = 3;       //The maximum number for current problem taken as input from the user
//const string SOC = string("d695");

int WIDTH = 16;
int parts[MAX_TAM_NUMBER+1];
int flag = 0;

bool PRINT_COREASSG  = 0;
bool PRINT_FINAL_COREASSG = 1;
bool PRINT_PARTITION_DETAILS= 0;
bool PRINT_PARTITIONS = 1;
//From window based power scheduling file
//# include "iolist.h"

#define SIZE1 50
#define LARGENUMBER 999999999

#define NO_OF_PARTICLES 2000
#define MAX_ITERATION 2000
#define ALPHA 0.1
#define BETA 0.1
#define TAM_WIDTH_MAX1 100 
#define BIT_LENGTH 4
#define MAX_NO_OF_INTERVAL 5000
#define TIME_INTERVAL 1000


typedef struct swapsequence{
int from;
int to;
} swapseq;


typedef struct {
        int corenum;
        int tam_width ;
        long int starttime ;
        long int endtime ;
} schedulerInfo ;

typedef struct
{
        int tam_list[TAM_WIDTH_MAX1];
        long int testtime_list[TAM_WIDTH_MAX1];
        int no_of_tam;
} tam_testtime_information;

typedef struct {
        long int time_instant[MAX_NO_OF_INTERVAL];
        double power_instant[MAX_NO_OF_INTERVAL];
        long int no_of_instans;
        //int core_power_index;
}core_time_power;

core_time_power core_time_power_info[SIZE1];
tam_testtime_information tam_testtime[SIZE1];
schedulerInfo scheduler [ NO_OF_PARTICLES ] [ SIZE1 ];
//particle partarray[NO_OF_PARTICLES];
//particle globalbest;
//schedulerInfo BestParticle [SIZE1];

double power_budget;
//int TAM_INFO[]= TAM;
int globalbestIndx; 

bool compare_by_word(const string& lhs, const string& rhs) {
	if(lhs.size() == rhs.size())
		return lhs< rhs;
	else
		return lhs.size()<rhs.size();
}
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    std::sort(files.begin(), files.end(), compare_by_word);
    closedir(dp);
    return 0;
}
void read_from_files(string dir, core cores[], tam tams[], times testtimes[][100])
{

	    vector<string> files = vector<string>();
	    string delims[MAX_SPACE_DELIM];
	    getdir(dir,files);
	    int tcore = 0;
	    for(int i = 0; i <MAX_SPACE_DELIM ; i++)
	    {
		    delims[i]  = string(" ");
		    for(int j=0; j <i; j++)	
		    {
			
			delims[i] = delims[i] + " ";
		    } 
	    }
	    for(unsigned int f = 0; f<files.size(); f++)
	    {
		  if(files[f].substr(files[f].find_last_of(".") + 1) == "txt")
		  {
			  ifstream fin;
			  string file = string(dir + "/" + files[f]);
			  cout<<file<<"\n";
			  fin.open(file.c_str());
			  if (!fin.good()) 
			  {
				  //cout<<dir + "/" + files[0];
				  continue;
			  }
			  while (!fin.eof())
			  {
				    char buf[MAX_CHARS_PER_LINE];
				    fin.getline(buf, MAX_CHARS_PER_LINE);
				    int n = 0; 
				    const char* token[MAX_TOKENS_PER_LINE] = {}; 
				    
				for(int i = 0; i<MAX_SPACE_DELIM ;i++)
				{
					//const char * DELIMITER = delims[i].c_str();
					token[n] = strtok(buf, DELIMITER); 
					if(token[n])
					{
						break;
					}
				}
				    if (token[0])
				    {
					      for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
					      {
						for(int i = 0; i<10;i++)
						{
							//const char * DELIMITER = delims[i].c_str();
							token[n] = strtok(0, DELIMITER); 
							if(token[n])
							{
								break;
							}
						}
					      }

			stringstream strvalue;
			strvalue << token[0];

			unsigned int intvalue;
			strvalue >> intvalue;
			//cout<<intvalue;

			stringstream strvalue2;
			strvalue2 << token[1];

			unsigned int intvalue2;
			strvalue2 >> intvalue2;
			//cout<<intvalue2;
					testtimes[tcore][intvalue-1].store_test(cores[tcore], tams[intvalue-1], intvalue2); 
						
					/*
					for (int i = 0; i < n; i++){ 
					      cout << "token[" << i << "] = " << token[i] ;
					  }

					    cout << endl;
					    */
				    }
			  }
			  tcore++;
		  }
	  }
	MAX_CORES = tcore;

	
}

void increment(int B, int j, int W)
{
	int bound;
	int sum = 0;
	for(int i=1; i<j;i++)
	{
		sum = sum + parts[i];
	}
	bound = (W - sum)/(B-j+1);
	//bound = W - sum;
	//bound = 9;
	if(parts[j]<bound)
	{
		parts[j]++;
		int sum = 0;
		for(int i=1;i<MAX_TAM_NUMBER_IP+1;i++)
		{
			if(i!=B)	
			{
				sum = sum+parts[i];
			}
		}
		parts[B] = W - sum;
		/*
		for(int i=0;i<=MAX_TAM_NUMBER+1;i++)
		{
			printf("%d ",parts[i]);
		}
		printf("\n");
		*/
		return;
	}
	else
	{
		if(j == 1 )
		{
			flag = 1;
			return;
		}
		else
		{
			increment(j,j-1,W);
		}
	}
}

int get_min_time(tam tams[])
{
	int min = tams[0].get_time();
	//printf("%d", min);
	int id = 0; int test;
	int max = 0;
	max = tams[0].get_width();
	int mid = 0;
	for(int i=1;i<WIDTH;i++)
	{
		//printf("%d\n", tams[i].get_time());
		//printf("%d\n", max);
		if(tams[i].get_time()<min)
		{
			min = tams[i].get_time(); 
			max = tams[i].get_width();
			id = i;
			mid = i;
		}
		else if(tams[i].get_time() == min)
		{
			test = -1;
			if(tams[i].get_width()>max )
			{
				max = tams[i].get_width();
				mid = i;
			}
		}
	}
	if(test == -1)
	{
		return mid;
	}
	else
	{
		return id;
	}


}

int get_max_core(core cores[], int tamid, times time[][MAX_TAM_NUMBER], tam tams[])
{

	int max = 0;
	int mid;int test;
	for(int i=0;i<MAX_CORES;i++)
	{
		if(!cores[i].is_assgn())
		{
			if(time[i][tamid].get_time()>max)
			{
				max = time[i][tamid].get_time();
				mid = i;
			}
			else if(time[i][tamid].get_time()==max)
			{
				test = -1;
			}
		}
	}
	if(test == -1)
	{
		int max = 0;
		int tid;
		for(int i=0; i<WIDTH;i++)
		{
			if(i!=tamid && tams[i].get_width()>max && tams[i].get_width() < tams[tamid].get_width())
			{
				max = tams[i].get_width();
				tid = i;
			}
		}
		int max2 = 0;int cid;
		for(int i=0; i<MAX_CORES;i++)
		{
			if(!cores[i].is_assgn())
			{
				if(time[i][tid].get_time()>max2)
				{
					cid = i;
					max2 = time[i][tid].get_time();
				}	
			}
		}
		return cid;
	}
	else
	{
		return mid;
	}
}

int core_assign(core cores[], tam tams[], times testtimes[][MAX_TAM_NUMBER])
{

	int check = 0;
	while(1)
	{
			int tamid = get_min_time(tams);
			int cid = get_max_core(cores, tamid, testtimes,tams);	
			if(PRINT_COREASSG)
			{
				printf("Core %d is assigned Tam %d whos test time is %d\n", cid, tamid, tams[tamid].get_time());
			}
			cores[cid].update_assg(tams[tamid]);
			tams[tamid].update_time(testtimes[cid][tamid].get_time());
			check++;
			
			//printf("%d\n", tams[tamid].get_time());
			//printf("%d\n", testing_times[cid][tamid]);
	if(check == MAX_CORES)
	{
		break;
	}
	}
	int tmax = 0;
	if(PRINT_COREASSG)
	{
		printf("Final core assignment:\nCore Tam TestingTime\n");
		for(int i=0; i<MAX_CORES;i++)
		{
			tam t = cores[i].get_core_tam();
			printf("%d     %d       %d\n",i,t.get_tam_id(),testtimes[i][t.get_tam_id()].get_time());

		}
		printf("Final total testing time on tams:\nTam Time\n");
	}
	for(int i=0; i<WIDTH;i++)
	{
		if(PRINT_COREASSG)
		{
			printf("%d   %d \n",i,tams[i].get_time());//,testtimes[i][t.get_tam_id()].get_time());
		}
		if(tams[i].get_time()>tmax)
		{
			tmax = tams[i].get_time(); 

		}

	}
	return tmax;
}
bool is_positive(int array[], int len)
{

	for(int i=1; i< len; i++)
	{
		if(array[i] <= 0)
		{
			return 0;
		}
	}
	return 1;
}

long int bin_packing(core cores[], times testtimes[][MAX_TAM_NUMBER])
{
	//int binary;
	const int TAM_WIDTH_MAX = MAX_TAM_WIDTH;
	const int SIZE = MAX_CORES;
	long int testtime[SIZE];
	int assigned_tam[SIZE];
	double area[SIZE];
	int schedule_index[SIZE];
	int no_of_core_scheduled = 0;
	//int tam_index[SIZE];
	long int break_point[SIZE+2];
	int available_tam_width[SIZE+2];
	int insert_index;
	long int temp;
	double max_weightage;
	int max_index;
	//long int current_time = 0;
	long int schedule_start_time;
	int temp_latest_power_index, latest_power_index ;
	int core_power_index;
	FILE *read_power_value;
	char arr[1000000], arr1[1000000], dumm[10];
	int index = 0;
	
	typedef struct {
		long int time_info[MAX_NO_OF_INTERVAL];
		double power_info[MAX_NO_OF_INTERVAL];
		//int running_index;
	} running_power_time;
	
	running_power_time running_power_time_info;
	
	
	for(int  i = 0; i< SIZE; i++)
		scheduler [index][i].starttime = -1;
	string Result;          // string which will contain the result
	ostringstream convert;   // stream used for the conversion
	convert << MAX_TAM_WIDTH;      // insert the textual representation of 'Number' in the characters in the stream
	Result = convert.str(); // set 'Result' to the contents of the stream
	string file = powerdir+string(Result)+string("/window_based_power_value/")+string("power_value_");
	/*for(int i = 0; i< SIZE; i++)
	{
		binary = 0;
		for(int k = 0; k < BIT_LENGTH ; k++)
			binary = binary + particle_info[(i* BIT_LENGTH) + k] * pow (10,BIT_LENGTH - 1 -k);
		tam_index[i] = bin_to_decimal(binary);
	}*/		
		
	/*for(int i = 0; i< SIZE; i++)
	{
		assigned_tam[i] = tam_testtime[i].tam_list[tam_index[i]];
		testtime[i] = tam_testtime[i].testtime_list[tam_index[i]];
		area[i] = assigned_tam[i] * testtime[i];
	}*/
	

	
	/*
	assigned_tam[0] = 7;
	testtime[0] = 77;
	assigned_tam[1] = 5;
	testtime[1] = 3161;
	assigned_tam[2] = 5;
	testtime[2] = 2507;
	assigned_tam[3] = 4;
	testtime[3] = 6782;
	assigned_tam[4] = 5;
	testtime[4] = 38481;
	assigned_tam[5] = 7;
	testtime[5] = 28199;
	assigned_tam[6] = 7;
	testtime[6] = 9695;
	assigned_tam[7] = 7;
	testtime[7] = 4605;
	assigned_tam[8] = 4;
	testtime[8] = 6597;
	assigned_tam[9] = 4;
	testtime[9] = 30132;
	*/
	for(int i = 0; i< SIZE; i++)
	{
		tam t = cores[i].get_core_tam();
		//available_tam_width[i] = t.get_width();
		assigned_tam[i] = t.get_width();//tam_testtime[i].tam_list[tam_index[i]];
		//tam_index[i] = t.get_width();
		//testtime[i] = t.get_time();//tam_testtime[i].testtime_list[tam_index[i]];
		testtime[i] = testtimes[i][t.get_tam_id()].get_time();
		area[i] = assigned_tam[i] * testtime[i];
	}
	for(int i = 0; i< SIZE; i++)
	{
		printf("Core %d -> %d Time -> %ld\n",i,assigned_tam[i],testtime[i]);  
	}
	
	for(int i = 0; i< SIZE; i++)
	{	
		for(int j= 0; j< MAX_NO_OF_INTERVAL; j++ )
		{
			core_time_power_info[i].time_instant[j] = 0;
			core_time_power_info[i].power_instant[j] = 0;
		}	
	}
	for(int i = 0; i< SIZE; i++)
	{

		char power_file[100];
		strcpy(power_file, file.c_str());
		char a[10];
		sprintf(a,"%d",i+1);
		strcat(power_file, a);
		strcat(power_file, ".txt");
		read_power_value = fopen(power_file,"r");
		
		//printf("\ntam index%d\n\n", tam_index[i]);
		for(int j = 0; j< ((assigned_tam[i]-1) * 2); j++)
			fscanf(read_power_value,"%[^\n]\n",arr);
		
		fscanf(read_power_value,"%[^\n]\n", arr1);	
		int k = 0 ;
		int temp_value;
		int current_index = 0;
		for( ; k < strlen(arr1) ; )
		{
			int j = 0 ;
			while( arr1[ k ] != '\t' )
			{
				dumm[ j ] = arr1[ k ] ;
				k ++ ;
				j ++ ;
			}
			dumm[ j ] = '\0' ;
			temp_value = atoi(dumm);
			core_time_power_info[i].time_instant[current_index] = temp_value;
			//printf("core %d - %d\n", i, temp_value);
			k ++ ;	
			current_index ++;
			
		}	
		
		core_time_power_info[i].no_of_instans = current_index;
		//printf("%d\n", core_time_power_info[i].no_of_instans);	
	
		
		fscanf(read_power_value,"%[^\n]\n", arr1);	
		k = 0 ;
		//int temp_value;
		current_index = 0;
		for( ; k < strlen(arr1) ; )
		{
			int j = 0 ;
			while( arr1[ k ] != '\t' )
			{
				dumm[ j ] = arr1[ k ] ;
				k ++ ;
				j ++ ;
			}
			dumm[ j ] = '\0' ;
			temp_value = atoi(dumm);
			core_time_power_info[i].power_instant[current_index ] = temp_value;
			//printf("core %d - %d\n", i, temp_value);
			k ++ ;	
			current_index ++;
			
		}
		fclose(read_power_value);
	}
		//exit(0);
	for(int i = 0; i< SIZE; i++)
			schedule_index[i] = 0;
			
	for(int i = 0; i<= SIZE+1; i++)
		available_tam_width[i] = TAM_WIDTH_MAX;
		
	running_power_time_info.time_info[0] = 0;
	running_power_time_info.power_info[0] = 0;
	for(int i = 1; i< MAX_NO_OF_INTERVAL; i++)
	{
		running_power_time_info.power_info[i] = 0;
		running_power_time_info.time_info[i] = running_power_time_info.time_info[i-1] + TIME_INTERVAL;  		
	}
	
	break_point[0] = 0;
	for(int i = 1; i<= SIZE+1; i++)
		break_point [i] = LARGENUMBER;
	
	int t =0;
	printf("\ni am here\n");
	//printf("\n i am here\n");	
	while (no_of_core_scheduled < SIZE)
	{
		int flag = 1;
		while(flag ==1)
		{
			max_weightage = 0;
			flag = 0;
			int jj = 0;
			schedule_start_time = break_point[jj];
		
			for (int kk = 0; kk< SIZE; kk++)
			{
				if(schedule_index[kk] == 0 )
				{
					//printf("\n%d\t%d\n",available_tam_width[jj],assigned_tam[kk]);
					if (available_tam_width[jj] >= assigned_tam[kk] )
					{
						latest_power_index = 0;
						while(running_power_time_info.time_info[latest_power_index] != break_point[jj])
							latest_power_index++;
						int check_flag = 1;
						long int temp_check_time = schedule_start_time;
						temp_latest_power_index = latest_power_index;
						core_power_index = 0;
	
						for(int aaa = 0; aaa< 100; aaa++)
						{
							//printf("%ld\t%lf\n", running_power_time_info.time_info[aaa], running_power_time_info.power_info[aaa]);
						}
						//printf("\n temp check time %ld\n", temp_check_time);
						while(core_power_index < core_time_power_info[kk].no_of_instans)
						{
							while(running_power_time_info.time_info[temp_latest_power_index]<=  temp_check_time + core_time_power_info[kk].time_instant[core_power_index])
							{	
								
								temp_latest_power_index++;
								
							}
							
							
							if(running_power_time_info.time_info[temp_latest_power_index] > temp_check_time + core_time_power_info[kk].time_instant[core_power_index]  && running_power_time_info.time_info[temp_latest_power_index] < temp_check_time + core_time_power_info[kk].time_instant[core_power_index + 1])
							{
								//printf("\n inside 1\n");
								while(running_power_time_info.time_info[temp_latest_power_index] > temp_check_time + core_time_power_info[kk].time_instant[core_power_index]  && running_power_time_info.time_info[temp_latest_power_index] < temp_check_time + core_time_power_info[kk].time_instant[core_power_index + 1])
								{
									if(running_power_time_info.power_info[temp_latest_power_index] + core_time_power_info[kk].power_instant[core_power_index+1] > power_budget || running_power_time_info.power_info[temp_latest_power_index + 1] + core_time_power_info[kk].power_instant[core_power_index+1] > power_budget)
									{
										
										check_flag = 0;
										break;
									}
									else
										temp_latest_power_index++;
								}
								if(check_flag == 0)
									break;
								else	
									core_power_index++;
							}
							
							
							if(running_power_time_info.time_info[temp_latest_power_index] > temp_check_time + core_time_power_info[kk].time_instant[core_power_index]  && running_power_time_info.time_info[temp_latest_power_index] == temp_check_time + core_time_power_info[kk].time_instant[core_power_index + 1])
							{
								//printf("\n inside 2\n");
								while(running_power_time_info.time_info[temp_latest_power_index] > temp_check_time + core_time_power_info[kk].time_instant[core_power_index]  && running_power_time_info.time_info[temp_latest_power_index] == temp_check_time + core_time_power_info[kk].time_instant[core_power_index + 1])
								{
									if(running_power_time_info.power_info[temp_latest_power_index] + core_time_power_info[kk].power_instant[core_power_index+1] > power_budget )
									{
										
										check_flag = 0;
										break;
									}
									else
										temp_latest_power_index++;
								}
								if(check_flag == 0)
									break;
								else	
									core_power_index++;
							}	
							
							//printf("\n %ld\n", running_power_time_info.time_info[temp_latest_power_index]);
							if(running_power_time_info.time_info[temp_latest_power_index] > temp_check_time + core_time_power_info[kk].time_instant[core_power_index + 1])
							{
								//printf("\n inside 3\n");
								if(running_power_time_info.power_info[temp_latest_power_index] + core_time_power_info[kk].power_instant[core_power_index+1] > power_budget )
								{
										
									check_flag = 0;
										
								}
								else
									temp_latest_power_index++;
							
								
								if(check_flag == 0)
									break;
								else	
									core_power_index++;
							}
							
							
							
						}
						
						
						if(check_flag == 1)
						{	
							flag = 1;
						
							if(area[kk]> max_weightage)
							{
								max_weightage = area[kk];
								max_index = kk;
								
							}
						}	
					}
				}
			}
			if (flag == 0)
			break;
			
			
			scheduler [index][t].corenum =  max_index;
			scheduler [index][t].tam_width = assigned_tam[max_index];
			scheduler [index][t].starttime = break_point[jj];
			scheduler [index][t].endtime = break_point[jj] + testtime [max_index];	
			
			
			
			schedule_start_time = break_point[jj];
			
			latest_power_index = 0;
			while(running_power_time_info.time_info[latest_power_index] != break_point[jj])
				latest_power_index++;
			temp_latest_power_index = latest_power_index;
			
			for(core_power_index = 0; core_power_index < core_time_power_info[max_index].no_of_instans; core_power_index++)
			{
				while((schedule_start_time + core_time_power_info[max_index].time_instant[core_power_index])!=  running_power_time_info.time_info[temp_latest_power_index ] && (schedule_start_time + core_time_power_info[max_index].time_instant[core_power_index]) > running_power_time_info.time_info[temp_latest_power_index ])
				{
					temp_latest_power_index++;
				}
				
				
				if((schedule_start_time + core_time_power_info[max_index].time_instant[core_power_index])!= running_power_time_info.time_info[temp_latest_power_index ])
				{
					for(int mm = MAX_NO_OF_INTERVAL - 1; mm > temp_latest_power_index; mm--)
					{
						running_power_time_info.time_info[mm] = running_power_time_info.time_info[mm-1];
						running_power_time_info.power_info[mm] = running_power_time_info.power_info[mm-1];
					}
					running_power_time_info.time_info[temp_latest_power_index] = schedule_start_time + core_time_power_info[max_index].time_instant[core_power_index];
					running_power_time_info.power_info[temp_latest_power_index] = running_power_time_info.power_info[temp_latest_power_index+1];
				}
			}	
			
			
			temp_latest_power_index = latest_power_index + 1;
			
			for(core_power_index = 0; core_time_power_info[max_index].time_instant[core_power_index] < testtime [max_index]; core_power_index++)
			{
				while(running_power_time_info.time_info[temp_latest_power_index]> (schedule_start_time + core_time_power_info[max_index].time_instant[core_power_index]) && running_power_time_info.time_info[temp_latest_power_index] <= (schedule_start_time + core_time_power_info[max_index].time_instant[core_power_index +1]))
				{
					running_power_time_info.power_info[temp_latest_power_index] = running_power_time_info.power_info[temp_latest_power_index] + core_time_power_info[max_index].power_instant[core_power_index +1];
					temp_latest_power_index++;
				}
			}
			
			
			
			int number_of_brk = 0;
			while(break_point [number_of_brk] != LARGENUMBER)
				number_of_brk++;
			break_point[number_of_brk] = scheduler [index][t].endtime;
			
			for (int i = 0; i<=(SIZE); i++)
				for (int j = 0; j<=(SIZE-i); j++)
				{
					if(break_point[j]> break_point[j+1])
					{
						temp = break_point[j];
						break_point[j] = break_point[j+1];
						break_point[j+1] = temp;
					}
				}
			for (int i = 0; i<= SIZE+1; i++)
			{
				if (break_point[i] == scheduler[index][t].endtime  )
				{
					insert_index = i;
					//break;
				}	
			}
			for (int i = number_of_brk; i>= insert_index; i--)
			{
				available_tam_width[i] = available_tam_width[i-1];
				
			}
			int a = jj;
			while(break_point[a]< scheduler[index][t].endtime )
			{		
				available_tam_width[a] = available_tam_width[a] - assigned_tam[max_index];
				a++;
			}	
			schedule_index[scheduler [index][t].corenum] = 1;
			
			
			
			no_of_core_scheduled++;	
			t++;
		}
		/*for(int aa = 0; aa <= SIZE+1; aa++)
		{
			printf("%d\t", available_tam_width[aa]);
		}
		printf("\n\n\n");
		
		for(int aa = 0; aa < SIZE; aa++)
			printf("%d\t", schedule_index[aa]);
		printf("\n\n\n");	
		*/	
		for (int i =0; i< SIZE+1; i++)
		{
			break_point[i] = break_point[i+1];
			available_tam_width[i] = available_tam_width[i+1];

		}
		break_point[SIZE+1] = LARGENUMBER;
		available_tam_width[SIZE+1] = TAM_WIDTH_MAX;
	}
	

	schedulerInfo tmp;
        for ( int i = 0 ; i < SIZE ; i ++ )
        {
                for ( int j = 0 ; j < SIZE-1-i ; j ++ ) 
                {
                        if ( scheduler [index][j+1].starttime <  scheduler [index][j].starttime ) 
                        {
                                tmp.corenum = scheduler [index][j+1]. corenum ;
                                tmp.tam_width = scheduler [index][j+1]. tam_width ;
                                tmp.starttime = scheduler [index][j+1]. starttime;
                                tmp.endtime = scheduler [index][j+1]. endtime;
                         
                                
                                scheduler [index][j+1]. corenum = scheduler [index][j]. corenum ;
                                scheduler [index][j+1]. tam_width = scheduler [index][j]. tam_width ;
                                scheduler [index][j+1]. starttime = scheduler [index][j]. starttime ;
                                scheduler [index][j+1]. endtime = scheduler [index][j]. endtime ;
				
				
                                scheduler [index][j]. corenum = tmp.corenum ;
                                scheduler [index][j]. tam_width = tmp.tam_width ;
                                scheduler [index][j]. starttime = tmp.starttime ;
                                scheduler [index][j]. endtime = tmp.endtime ;
                   
                        }
                }
        }
	for ( int ii = 0 ; ii < SIZE ; ii ++ ) 
                printf ("{ %d, %d, %ld, %ld }\n", scheduler [index][ii].corenum, scheduler [index][ii].tam_width, scheduler [index][ii]. starttime, scheduler [index][ii]. endtime);
	/*int new_tam_index[SIZE];
	for(int i = 0; i< SIZE; i++)
	{
		for(int j = 0; j< tam_testtime[scheduler [index][i].corenum].no_of_tam; j++)
		{
			if(scheduler [index][i].tam_width == tam_testtime[scheduler [index][i].corenum].tam_list[j])
			{
				new_tam_index[scheduler [index][i].corenum] = j;
			}
		}
	}
	
	for(int i = 0; i< SIZE; i++)
	{
		particle_info[i] = (double)new_tam_index[i] / (double)tam_testtime[i].no_of_tam;
	}
	*/
	/*int new_tam;
	for(int i = 0; i< SIZE; i++)
	{
		new_tam = tam_testtime[i]. tam_list[tam_index[i]];
		printf("\nnew tam %d\n", new_tam);
		//printf()
		int ll = BIT_LENGTH -1;
		while(new_tam != 0)
		{
			printf("\n *****\n");
			particle_info[i*BIT_LENGTH + ll] = new_tam % 2;
			new_tam = new_tam / 2;
			ll--;
		}
		
	}
	printf("\n++++\n");*/
	int flag = 0;
	int kk;
	for ( kk = 0; kk<= SIZE+1; kk++)
		if(break_point[kk] == LARGENUMBER)
		{	
			flag = 1;
			break;
		}
	if (flag ==1)
	{
	
		if (break_point[kk-1] == 64)
		{	
			printf("\n********%ld\n",break_point[kk-1]);
			for(int i = 0; i<= SIZE+1; i++)
				printf("%ld\t", break_point[i]);
			printf("\n\n");
			for(int i = 0; i<= SIZE+1; i++)
				printf("%d\t", available_tam_width[i]);
			printf("\n\n");
			exit(0);
		}
		
		return(break_point[kk-1] );
	}
	else
	{
	
		if (break_point[kk] == 64)
		{	
			printf("\n++++++%ld\n",break_point[kk]);
			for(int i = 0; i<= SIZE+1; i++)
				printf("%ld\t", break_point[i]);
			printf("\n\n");
			for(int i = 0; i<= SIZE+1; i++)
				printf("%d\t", available_tam_width[i]);
			printf("\n\n");
			exit(0);
		}
		
		return (break_point[kk]);
	}
	
				
}

int main ()
{


	vector<string> socs = vector<string>();
	getdir(dirsoc, socs);
	cout<<"Enter choice:\n";
 	for(unsigned int f = 0; f<socs.size(); f++)
	    {
		    if(strcmp(socs[f].c_str(),".")&&strcmp(socs[f].c_str(),".."))
		cout<<f<<". "<<socs[f].c_str()<<"\n";
	    }
	int choice;
	cin>>choice;
		cout<<"Enter maximum tam width:\n";
		cin>>MAX_TAM_WIDTH;
		cout<<"Enter maximum tam number:\n";
		cin>>MAX_TAM_NUMBER_IP;
		string Result;          // string which will contain the result
		ostringstream convert;   // stream used for the conversion
		convert << MAX_TAM_WIDTH;      // insert the textual representation of 'Number' in the characters in the stream
		Result = convert.str(); // set 'Result' to the contents of the stream
		string dir = dirsoc + socs[choice]+string("/")+string(Result)+string("/balanced_wrapper/full");
		powerdir = powerdir+socs[choice]+string("/");
			
		core cores[MAX_CORES];
		tam tams[MAX_TAM_WIDTH];
		times testtimes[MAX_CORES][100];
		for(int i=0; i<MAX_CORES;i++)
		{
			cores[i].store_id(i);
		}
		for(int i=0; i<MAX_TAM_WIDTH;i++)
		{
			tams[i].store_tam(i,i+1);
		}

		    read_from_files(dir, cores, tams, testtimes);
		cout<<"Enter Max Power:\n";
		cin>>power_budget;
		    //power_budget = atof (argv[1]);
   cout<<"     "; 
    for (int m=0;m<MAX_TAM_WIDTH;m++)
	    {
		cout<<m+1<<" ";
	    }
	    cout<< "\n";

	for (int n=0;n<MAX_CORES;n++)
	{
		cout<<n<<"   ";
	    for (int m=0;m<MAX_TAM_WIDTH;m++)
	    {
		cout<<testtimes[n][m].get_time()<<" ";
	    }
	    cout<< "\n";
	}

	cout<<"Enter Partitions manually(1/0)?\n";
	cin>>choice;
	if(choice == 1)
	{
		 PRINT_COREASSG  = 1;
		 PRINT_FINAL_COREASSG = 1;
		 PRINT_PARTITION_DETAILS= 1;
		 PRINT_PARTITIONS = 1;

		core cores_now[MAX_CORES];
		
		tam tams_now[MAX_TAM_NUMBER_IP];
		times testtimes_now[MAX_CORES][MAX_TAM_NUMBER];

		cout<<"Enter tam partitions:\n";
		int temp = 1;

		for(int i=1;i<=MAX_TAM_NUMBER_IP;i++)
			{
				cin>>temp;	
			 	parts[i] = temp;	
			}
		for(int i=1;i<=MAX_TAM_NUMBER_IP;i++)
			{
				
				tams_now[i-1].store_tam(i-1, parts[i]);	
				
			}

		for(int i=0; i<MAX_CORES;i++)
			{
				cores_now[i].store_id(i);
			}

		for(int i=0;i<MAX_TAM_NUMBER_IP;i++)
				{

					if(PRINT_PARTITIONS)
					{
						cout<<"         "<<tams_now[i].get_width();
					}
					//tams_now[i] = tams[parts[i]-1];	
					
				}
				if(PRINT_PARTITIONS)
				{
					cout << "\n";	
				}
		for(int n=0; n<MAX_CORES; n++)
			{
				if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
				{
					cout<<"Core "<<n<<" ";
				}
				for(int m=1; m<=MAX_TAM_NUMBER_IP;m++)
				{
					testtimes_now[n][m-1] = testtimes[n][parts[m]-1];

					if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
					{
						cout<<testtimes_now[n][m-1].get_time()<<" ";
					}
				}

				if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
				{
					cout<<"\n";
				}
			}
		WIDTH = MAX_TAM_NUMBER_IP;   //The limit to which testtimes_now array exists

		int time_now = core_assign(cores_now, tams_now, testtimes_now);
		if(PRINT_COREASSG)
		{
			cout<<"Testtime "<<time_now;
		}
			
		long int time = bin_packing(cores_now, testtimes_now);
		exit(0);

	}

	
	int lc = 2;
	int tbest = 100000000;
	core cores_best[MAX_CORES];
	tam tams_best[MAX_TAM_NUMBER];
	times testtimes_best[MAX_CORES][MAX_TAM_NUMBER];
	int lcf;
	while(lc!=MAX_TAM_NUMBER_IP+1)
	{
		
		for(int i=1;i<=lc-2;i++)
		{
			parts[i] = 1;
		}
		parts[lc-1] = 0;
		flag = 0;
		while(flag != 1)
		{
			increment(lc,lc-1,MAX_TAM_WIDTH);
			core cores_now[MAX_CORES];
				
				tam tams_now[lc];
				times testtimes_now[MAX_CORES][MAX_TAM_NUMBER];
				for(int i=1;i<=lc;i++)
				{
					//printf("%d ",parts[i]);
					//tams_now[i-1] = tams[parts[i]-1];
					tams_now[i-1].store_tam(i-1, parts[i]);	
					
				}

				for(int i=0; i<MAX_CORES;i++)
				{
					cores_now[i].store_id(i);
				}

				/** Printing stuff **/
				for(int i=0;i<lc;i++)
				{

					if(PRINT_PARTITIONS)
					{
						cout<<"         "<<tams_now[i].get_width();
					}
					//tams_now[i] = tams[parts[i]-1];	
					
				}
				if(PRINT_PARTITIONS)
				{
					cout << "\n";	
				}
				for(int n=0; n<MAX_CORES; n++)
				{
					if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
					{
						cout<<"Core "<<n<<" ";
					}
					for(int m=1; m<=lc;m++)
					{
						testtimes_now[n][m-1] = testtimes[n][parts[m]-1];

						if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
						{
							cout<<testtimes_now[n][m-1].get_time()<<" ";
						}
					}

					if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
					{
						cout<<"\n";
					}
				}
				//printf("\n");
				WIDTH = lc;   //The limit to which testtimes_now array exists

				int time_now = core_assign(cores_now, tams_now, testtimes_now);
				if(PRINT_COREASSG)
				{
					cout<<"Testtime "<<time_now;
				}
				if(time_now <= tbest)
				{
					tbest = time_now;
					for(int i=1;i<=lc;i++)
					{
						//printf("%d ",parts[i]);
						//tams_now[i-1] = tams[parts[i]-1];
						tams_best[i-1]=tams_now[i-1];	
						
					}

					for(int i=0; i<MAX_CORES;i++)
					{
						cores_best[i]=cores_now[i];
					}
					for(int n=0; n<MAX_CORES; n++)
					{
						for(int m=1; m<=lc;m++)
						{
							testtimes_best[n][m-1] = testtimes_now[n][m-1] ;
						}
					}
					lcf= lc;	
				}
			

				if(PRINT_COREASSG || PRINT_PARTITIONS)
				{
					cout<<"\n-----------------------------------------------------------------\n";
				}
			//}
		}
		lc++;

	}
	printf("Lowest test time Final core assignment for %d :\nCore Tam TestingTime\n", tbest);
	for(int i=0; i<MAX_CORES;i++)
	{
		tam t = cores_best[i].get_core_tam();
		printf("%d     %d       %d\n",i,t.get_tam_id(),testtimes_best[i][t.get_tam_id()].get_time());
		//printf("%d->%d ",i,t.get_tam_id());

	}
	printf("Final total testing time on tams:\nTam Width   Time\n");
	for(int i=0; i<lcf;i++)
	{
		printf("%d   %d   %d \n",i,tams_best[i].get_width(),tams_best[i].get_time());//,testtimes[i][t.get_tam_id()].get_time());
	}

	long int total_time = bin_packing(cores_best, testtimes_best);
	//cout<<"The best assignment is :"<<core_assign(cores_best, tams_best, testtimes_best);
	
    return 0;

}


