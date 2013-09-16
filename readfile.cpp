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


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 3;
const char* const DELIMITER = "	";
const int MAX_SPACE_DELIM = 10;
int MAX_CORES = 50;
int MAX_TAM_WIDTH = 16;
const int MAX_TAM_NUMBER = 100;  //Used for array size constants
int MAX_TAM_NUMBER_IP = 3;       //The maximum number for current problem taken as input from the user
const string SOC = string("d695");

int WIDTH = 16;
int parts[MAX_TAM_NUMBER+1];
int flag = 0;

bool PRINT_COREASSG  = 0;
bool PRINT_FINAL_COREASSG = 1;
bool PRINT_PARTITION_DETAILS= 0;
bool PRINT_PARTITIONS = 1;

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

int main ()
{


	vector<string> socs = vector<string>();
	string dirsoc = string("/home/aditya25/Desktop/BTP/SOC_testing/");
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
		string dir = string("/home/aditya25/Desktop/BTP/SOC_testing/")+socs[choice]+string("/")+string(Result)+string("/balanced_wrapper/full");

			
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
		//printf("%d     %d       %d\n",i,t.get_tam_id(),testtimes_best[i][t.get_tam_id()].get_time());
		printf("%d->%d ",i,t.get_tam_id());

	}
	printf("Final total testing time on tams:\nTam Width   Time\n");
	for(int i=0; i<lcf;i++)
	{
		printf("%d   %d   %d \n",i,tams_best[i].get_width(),tams_best[i].get_time());//,testtimes[i][t.get_tam_id()].get_time());
	}

	//cout<<"The best assignment is :"<<core_assign(cores_best, tams_best, testtimes_best);
	
    return 0;

}


