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


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 2;
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
	printf("Reading from %s \n", file.c_str());
	ifstream fin;
	int line_count = 0;
	int power_area = 0;
	fin.open(file.c_str());
			  if (!fin.good()) 
			  {
				  cout<<"Error opening file\n";
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
					      cout << "token[" << i << "] = " << token[i] ;
					      
					}

					cout << endl;
					    
			  }
			  printf("Power consumed %d\n", power_area);
			  return power_area;
}
int main ()
{

	int time_limit = 0;
	vector<string> socs = vector<string>();
	string dirsoc = string("/home/aditya/BTP/power_values/");
	getdir(dirsoc, socs);
	for(unsigned int f = 0; f<socs.size(); f++)
	    {
		    if(strcmp(socs[f].c_str(),".")&&strcmp(socs[f].c_str(),".."))
		cout<<f<<". "<<socs[f].c_str()<<"\n";
	    }
	int choice, core, tamw;
	cin>>choice;
	cout<<"Enter core number:\n";
	cin>>core;
	cout<<"Enter tam width:\n";
	cin>>tamw;
	cout<<"Enter test time (limit):\n";
	cin>>time_limit;

	string corenum;          // string which will contain the result
	ostringstream convert;   // stream used for the conversion
	convert << core;      // insert the textual representation of 'Number' in the characters in the stream
	corenum = convert.str(); // set 'Result' to the contents of the stream
	
	string tamwd;          // string which will contain the result
	ostringstream convert2;   // stream used for the conversion
	convert2 << tamw;      // insert the textual representation of 'Number' in the characters in the stream
	tamwd = convert2.str(); // set 'Result' to the contents of the stream

	string file = dirsoc+socs[choice]+string("/cycle_wise_power_value/")+string("cycle_accurate_power_module_no_")+string(corenum)+string("tam_width_")+string(tamwd)+string(".txt");
	printf("Reading from %s \n", file.c_str());
	ifstream fin;
	int line_count = 0;
	int power_area = 0;
	fin.open(file.c_str());
			  if (!fin.good()) 
			  {
				  cout<<"Error opening file\n";
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
					      cout << "token[" << i << "] = " << token[i] ;
					      
					}

					cout << endl;
					    
			  }
			  printf("Power consumed %d:\n", power_area);
			  
    	return 0;

}


