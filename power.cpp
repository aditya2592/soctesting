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
#include "soctest_power.h"
#include <sstream>
#include <algorithm>


string powerdir = string("/home/aditya25/btp/power_values/");
string dirsoc = string("/home/aditya25/btp/SOC_testing/");

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 3;
const char* const DELIMITER = "	";
const int MAX_SPACE_DELIM = 10;
int MAX_CORES = 50;		//Used for array size constants	
int MAX_TAM_WIDTH = 16;		//Taken as input from user later
const int MAX_TAM_NUMBER = 100;  //Used for array size constants
int MAX_TAM_NUMBER_IP = 3;       //The maximum number for current problem taken as input from the user

int WIDTH = 16;
int parts[MAX_TAM_NUMBER+1];
int flag = 0;

bool PRINT_COREASSG  = 0;
bool PRINT_FINAL_COREASSG = 1;
bool PRINT_PARTITION_DETAILS= 0;
bool PRINT_PARTITIONS = 1;

#define MAX_NO_OF_INTERVAL 500000 
typedef struct {
	long int time_instant[MAX_NO_OF_INTERVAL];
	int power_instant[MAX_NO_OF_INTERVAL];
	long int no_of_instants;
}core_time_power;
core_time_power core_time_power_info;
int POWER_MAX;


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
int get_power_consumed(int core, int tamw, int time_limit, string dirsoc)
{
	

	string corenum;          // string which will contain the result
	ostringstream convert;   // stream used for the conversion
	convert << core;      // insert the textual representation of 'Number' in the characters in the stream
	corenum = convert.str(); // set 'Result' to the contents of the stream
	
	string tamwd;          // string which will contain the result
	ostringstream convert2;   // stream used for the conversion
	convert2 << tamw;      // insert the textual representation of 'Number' in the characters in the stream
	tamwd = convert2.str(); // set 'Result' to the contents of the stream

	string file = dirsoc+string("/cycle_wise_power_value/")+string("cycle_accurate_power_module_no_")+string(corenum)+string("tam_width_")+string(tamwd)+string(".txt");
	//printf("Reading power data from %s \n", file.c_str());
	ifstream fin;
	int line_count = 0;
	int power_area = 0;
	fin.open(file.c_str());
			  if (!fin.good()) 
			  {
				  cout<<"Error opening file "<<file.c_str()<<"\n";
			  }
			  while (!fin.eof())
			  {
				    char buf[MAX_CHARS_PER_LINE];
				    fin.getline(buf, MAX_CHARS_PER_LINE);	//Reading entire line into buffer
				    int n = 0; 
				    const char* token[MAX_TOKENS_PER_LINE] = {}; 
				    line_count++;
				if(line_count==1)
				{
					continue;
				}
				
				   
				//Reading the first value of the line into array   
				for(int i = 0; i<MAX_SPACE_DELIM ;i++)
				{
					token[n] = strtok(buf, DELIMITER); 
					if(token[n])
					{
						break;
					}
				}
				    //Reading rest of the words in the line separated by spaces into the array
				    if (token[0])
				    {
					      for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
					      {
						for(int i = 0; i<10;i++)
						{
							token[n] = strtok(0, DELIMITER); 
							if(token[n])
							{
								break;
							}
						}
					      }
				    }
				    	//Converting token values read from file to integers
					stringstream strvalue;
					strvalue << token[0];
					 int intvalue;
					strvalue >> intvalue;


					stringstream strvalue2;
					strvalue2 << token[1];

					 int intvalue2;
					strvalue2 >> intvalue2;
					
					//Integrating by addding power values upto the test time limit taken
					if(intvalue<=time_limit)
					      {
					      	power_area = power_area + intvalue2;
					      }
					      
					  
					//Test token values read from file    
					for (int i = 0; i < n; i++)
					{ 
					      //cout << "token[" << i << "] = " << token[i] ;
					      
					}

					//cout << endl;
					    
			  }
			  //printf("Power consumed %d\n", power_area);
			  return power_area;
}
void check_assignments(core cores[])
{
	
	/*
	for(int n=0; n<MAX_TAM_NUMBER_IP;n++)
	{
		//printf("Tam ID - %d of Width - %d has cores:", n, tams[n].get_width());
		ass_core = tams[n].get_cores();	
		for(int m=0; m<MAX_CORES;m++)
		{
			//printf("Core:%d ", *ass_core[m].get_id());
		}
		
	}
	*/
	for(int m=0; m<MAX_CORES;m++)
		{
			tam t = cores[m].get_core_tam();
			printf("Core %d assigned to tam:%d\n", m,t.get_tam_id());
		}
	
}
bool check_all_cores_assgn(core cores[])
{

	for(int i=0;i<MAX_CORES;i++)
	{
		if(cores[i].is_assgn()==0)
		{
			return	0;				
		}
	}
	return 1;
}
void update_power_info(int core, int tamw, int start_time, int end_time, bool test)
{
	string corenum;          // string which will contain the result
	ostringstream convert;   // stream used for the conversion
	convert << core;      // insert the textual representation of 'Number' in the characters in the stream
	corenum = convert.str(); // set 'Result' to the contents of the stream
	
	string tamwd;          // string which will contain the result
	ostringstream convert2;   // stream used for the conversion
	convert2 << tamw;      // insert the textual representation of 'Number' in the characters in the stream
	tamwd = convert2.str(); // set 'Result' to the contents of the stream

	string file = powerdir+string("/cycle_wise_power_value/")+string("cycle_accurate_power_module_no_")+string(corenum)+string("tam_width_")+string(tamwd)+string(".txt");
	//printf("Reading power data from %s \n", file.c_str());
	ifstream fin;
	int line_count = 0;
	fin.open(file.c_str());
	if (!fin.good()) 
	{
		cout<<"Error opening file "<<file.c_str()<<"\n";
	}
	while (!fin.eof())
	{
		char buf[MAX_CHARS_PER_LINE];
		fin.getline(buf, MAX_CHARS_PER_LINE);	//Reading entire line into buffer
		int n = 0; 
		const char* token[MAX_TOKENS_PER_LINE] = {}; 
		line_count++;
		if(line_count==1)
		{
			continue;
		}


		//Reading the first value of the line into array   
		for(int i = 0; i<MAX_SPACE_DELIM ;i++)
		{
			token[n] = strtok(buf, DELIMITER); 
			if(token[n])
			{
				break;
			}
		}
		//Reading rest of the words in the line separated by spaces into the array
		if (token[0])
		{
			for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
			{
				for(int i = 0; i<10;i++)
				{
					token[n] = strtok(0, DELIMITER); 
					if(token[n])
					{
						break;
					}
				}
			}
		}
		//Converting token values read from file to integers
		stringstream strvalue;
		strvalue << token[0];
		int intvalue;
		strvalue >> intvalue;  //clock cycle


		stringstream strvalue2;
		strvalue2 << token[1];

		int intvalue2;
		strvalue2 >> intvalue2;  //power value of this cycle
		if(start_time+intvalue<end_time)
		{
			core_time_power_info.power_instant[start_time+intvalue] = core_time_power_info.power_instant[start_time+intvalue] + intvalue2;     
		}
		//Test token values read from file    
		for (int i = 0; i < n; i++)
		{ 
			//cout << "token[" << i << "] = " << token[i] ;
		}

		//cout << endl;

	}
	//printf("Power consumed %d\n", power_area);
}
void print_power_all(int tsched_max)
{

	for(int i=0; i<=tsched_max; i++)
	{
		if(core_time_power_info.power_instant[i])
		{
			printf("%d : %d ",i, core_time_power_info.power_instant[i]);
		}		
	}	
	cout<<"\n";

}
bool check_power_constraint(int core, int tamw, int start_time, int end_time, bool test)
{
	string corenum;          // string which will contain the result
	ostringstream convert;   // stream used for the conversion
	convert << core;      // insert the textual representation of 'Number' in the characters in the stream
	corenum = convert.str(); // set 'Result' to the contents of the stream
	
	string tamwd;          // string which will contain the result
	ostringstream convert2;   // stream used for the conversion
	convert2 << tamw;      // insert the textual representation of 'Number' in the characters in the stream
	tamwd = convert2.str(); // set 'Result' to the contents of the stream

	string file = powerdir+string("/cycle_wise_power_value/")+string("cycle_accurate_power_module_no_")+string(corenum)+string("tam_width_")+string(tamwd)+string(".txt");
	//printf("Reading power data from %s \n", file.c_str());
	ifstream fin;
	int line_count = 0;
	fin.open(file.c_str());
	if (!fin.good()) 
	{
		cout<<"Error opening file "<<file.c_str()<<"\n";
	}
	while (!fin.eof())
	{
		char buf[MAX_CHARS_PER_LINE];
		fin.getline(buf, MAX_CHARS_PER_LINE);	//Reading entire line into buffer
		int n = 0; 
		const char* token[MAX_TOKENS_PER_LINE] = {}; 
		line_count++;
		if(line_count==1)
		{
			continue;
		}


		//Reading the first value of the line into array   
		for(int i = 0; i<MAX_SPACE_DELIM ;i++)
		{
			token[n] = strtok(buf, DELIMITER); 
			if(token[n])
			{
				break;
			}
		}
		//Reading rest of the words in the line separated by spaces into the array
		if (token[0])
		{
			for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
			{
				for(int i = 0; i<10;i++)
				{
					token[n] = strtok(0, DELIMITER); 
					if(token[n])
					{
						break;
					}
				}
			}
		}
		//Converting token values read from file to integers
		stringstream strvalue;
		strvalue << token[0];
		int intvalue;
		strvalue >> intvalue;  //clock cycle


		stringstream strvalue2;
		strvalue2 << token[1];

		int intvalue2;
		strvalue2 >> intvalue2;  //power value of this cycle
		if(test && start_time+intvalue<end_time)
		{
			if(core_time_power_info.power_instant[start_time+intvalue] + intvalue2 > POWER_MAX)
			{
				return 0;
			}
		}

		//Test token values read from file    
		for (int i = 0; i < n; i++)
		{ 
			//cout << "token[" << i << "] = " << token[i] ;
		}

		//cout << endl;

	}

	return 1;
}
int main ()
{

	
	vector<string> socs = vector<string>();
	getdir(dirsoc, socs);
	cout<<"Enter choice:\n";
	//Getting choice of SoCs from the directory names inside the SoC_testing folder
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
		string dir = dirsoc+socs[choice]+string("/")+string(Result)+string("/balanced_wrapper/full");
		powerdir = powerdir+socs[choice];
			
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

		//Reading core, corresponding test times for the maximum tam width entered
		    read_from_files(dir, cores, tams, testtimes);

		cout<<"Enter Max Power:\n";
		cin>>POWER_MAX;

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
		times testtimes_now[MAX_CORES][MAX_TAM_NUMBER];			//Will contain testtimes for each core for different tams in current partition scheme

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
		//Printing the current parition scheme
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
		cout<<"-------------------------------------------------------------------\n";
		//End of printing parition scheme
		
		WIDTH = MAX_TAM_NUMBER_IP;   //The limit to which testtimes_now array exists

		 PRINT_PARTITION_DETAILS= 1;
		 PRINT_PARTITIONS = 1;
		int PRINT_POWER_DETAIL = 1;
		int PRINT_TAM_ASSGNS = 1;

		int tsched_max = 0;//int tmax;
		while(!check_all_cores_assgn(cores_now))
		{	
			//tams_now[1].update_time(100);

			//Finding the tam with maximum scheduled time
			for(int n=0; n<MAX_TAM_NUMBER_IP; n++)
			{
				int time = tams_now[n].get_time();
				if(time>tsched_max)
				{
					tsched_max = time;
					//tmax = n;
				}
			}
			
			//print_power_all(tsched_max);
			//Find the core, tam pair satisfying the condition of minimum gap between max time tam edge and that tam+core
			int tmin = 1000000; int min_core; int min_tam;	
			for(int n=0; n<MAX_CORES; n++)
				{
					if(!cores_now[n].is_assgn())
					{
						if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
						{
							cout<<"Core "<<n<<" ";
						}
						for(int m=1; m<=MAX_TAM_NUMBER_IP;m++)
						{
								int temp = tsched_max-tams_now[m-1].get_time()-testtimes_now[n][m-1].get_time();
								bool pow_cond = 1;
								//pow_cond = check_power_constraint(n+1, tams_now[m-1].get_width(),tams_now[m-1].get_time(), tams_now[m-1].get_time()+testtimes_now[n][m-1].get_time(),1);
								if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
								{
									if(!pow_cond)
										cout<<temp<<"(p) ";
									else
										cout<<temp<<" ";
								}
								if(temp<tmin && pow_cond && temp>0)
								{
									tmin = temp;
									min_core = n;
									min_tam = m-1;
								}

								if(!pow_cond)
								{
									//Print cores and tams that violate the power max condition
									//cout<<"core:"<<n<<" "<<m-1<<"\n";
								}
						}	
		
						if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
						{
							cout<<"\n";
						}
					}
				}
			if(tmin == 1000000)
			{
				//No such pair condition. Reverse minimum gap to find the right tam
				cout<<"Condition one not satisfied\n";
				tmin = 1000000;
				int min_tam;
				for(int n=0; n<MAX_CORES; n++)
					{
						if(!cores_now[n].is_assgn())
						{
							if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
							{
								cout<<"Core "<<n<<" ";
							}
							for(int m=1; m<=MAX_TAM_NUMBER_IP;m++)
							{

								int temp = tams_now[m-1].get_time()+testtimes_now[n][m-1].get_time()-tsched_max;

								bool pow_cond = 1;
								//pow_cond = check_power_constraint(n+1, tams_now[m-1].get_width(),tams_now[m-1].get_time(), tams_now[m-1].get_time()+testtimes_now[n][m-1].get_time(),1);
								if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
								{
									if(!pow_cond)
										cout<<temp<<"(p) ";
									else
										cout<<temp<<" ";
								}
								if(temp<tmin && pow_cond)
								{
									tmin = temp;
									min_tam = m-1;
								}

								if(!pow_cond)
								{
									//cout<<"core:"<<n<<" "<<m-1<<"\n";
								}
							}

							if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
							{
								cout<<"\n";
							}
						}
					}
		

				//cout<<min_core.get_id();   //Checking the min gap core
				//cout<<min_tam.get_tam_id();  //Checking the min gap tam corresponsing to min core above
				if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
					cout<<"Selected tam:"<<min_tam<<"\n";
				//Valid tam found according to given condition. Checking for core with maximum power area for given tam
				int max_power = 0;
				int min_core_new; 
				for(int n=0; n<MAX_CORES; n++)
				{
					if(!cores_now[n].is_assgn())
					{
						int temp = get_power_consumed(n+1,tams_now[min_tam].get_width(),testtimes_now[n][min_tam].get_time(),powerdir);
						if(PRINT_POWER_DETAIL)
						{
							printf("Power for core %d and tam width %d till %d:%d\n", n, tams_now[min_tam].get_width(), testtimes_now[n][min_tam].get_time(), temp);
						}
						if(temp>max_power)
						{
							max_power = temp;
							min_core_new = n;
						}
					}
					
				}

				if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
					cout<<"Selected core with max power area:"<<min_core_new<<"\n";
				
				//Core with max power consumed under selected tam has been found. Assign the core to the tam
				int temp = testtimes_now[min_core_new][min_tam].get_time();
				update_power_info(min_core_new+1, tams_now[min_tam].get_width(),tams_now[min_tam].get_time(), tams_now[min_tam].get_time()+temp,0); //Update the power versus time values	
				tams_now[min_tam].put_core(temp, min_core_new);	//Update the test time on tam
				cores_now[min_core_new].update_assg(tams_now[min_tam]);	//Update tam assignment for core

				//check_assignments(cores_now);
			}
			else
			{
				//Valid core, tam pair found. Make core assignment to tam

				if(PRINT_PARTITIONS && PRINT_PARTITION_DETAILS)
					cout<<"Selected tam:"<<min_tam<<"and core:"<<min_core<<"\n";
				int temp = testtimes_now[min_core][min_tam].get_time();
				update_power_info(min_core+1, tams_now[min_tam].get_width(),tams_now[min_tam].get_time(), tams_now[min_tam].get_time()+temp,0); //Update the power versus time values	
				tams_now[min_tam].put_core(temp, min_core);
				cores_now[min_core].update_assg(tams_now[min_tam]);
					
				
			}
			for(int i=0; i<MAX_TAM_NUMBER_IP; i++)
			{
				int * ass_cores = tams_now[i].get_cores();
				if(PRINT_TAM_ASSGNS)
					printf("%d:", i);
				int j = 0;
				while(ass_cores[j]!=-1)
				{					
					if(PRINT_TAM_ASSGNS)
						cout<<ass_cores[j]<<" ";
					j++;
				}
				if(PRINT_TAM_ASSGNS)
					printf("        Time : %d\n", tams_now[i].get_time());
			}
			cout<<"\n";
		}
		exit(0);

	}
	
    return 0;

}


