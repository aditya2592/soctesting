# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <time.h>
#include <iostream>
#include <fstream>
using std::ifstream;
#include <cstring>
using namespace std;
#include <sstream>
#include <algorithm>


int INTERVAL;
//#define INTERVAL 1000
void power_value_calculator (int module_no, int TAM_WIDTH_MAX)
{
	long int counter;
	long int max_power_at_interval;
	long int time_power_store[10000][2];
	FILE *read_input, *write_output;
	long int no_of_clk_cycle;
	long int prev_no_of_clk_cycle;
	long int clk_cycle;
	long int power;
	long int time;
	long int time_power_index;
	long int current_time;
	long int time_index;
	
		string Result;          // string which will contain the result
		ostringstream convert;   // stream used for the conversion
		convert << TAM_WIDTH_MAX;      // insert the textual representation of 'Number' in the characters in the stream
		Result = convert.str(); // set 'Result' to the contents of the stream
	
	char output_file [200] = "/media/aditya25/E23CD82F3CD7FD0B/BTP/SOC_testing/p93791/";
	strcat(output_file, Result.c_str());
	strcat(output_file,"/window_based_power_value/power_value_");
	char a[10];
	sprintf(a,"%d", module_no);
	strcat(output_file, a);
	strcat(output_file, ".txt");
	write_output = fopen(output_file,"w");
	prev_no_of_clk_cycle = 0;
	for(int i = 1; i<=  TAM_WIDTH_MAX;)
	{
		char input_file [200] = "/media/aditya25/E23CD82F3CD7FD0B/BTP/power_values/p93791/cycle_wise_power_value/cycle_accurate_power_module_no_";
		char r[10];
		sprintf(r,"%d", module_no);
		strcat (input_file,r);
		strcat(input_file,"tam_width_");
		char r1[10];
		sprintf(r1,"%d", i);
		strcat(input_file, r1);
		strcat(input_file, ".txt");
		read_input = fopen(input_file,"r");
		fscanf(read_input,"%ld\n", &no_of_clk_cycle);
		
		time_power_index = 0;
		time_power_store[time_power_index][0] = 0;
		time_power_store[time_power_index][1] = 0;
		time_power_index++;
		//current_time = 0;
		time_index = 0;
		if(no_of_clk_cycle != prev_no_of_clk_cycle || no_of_clk_cycle == prev_no_of_clk_cycle)
		{	
			while (time_index < no_of_clk_cycle)
			{
				counter = 1;
				max_power_at_interval = 0;
				while(counter <= INTERVAL && time_index < no_of_clk_cycle)
				{
					fscanf(read_input,"%ld\t%ld\n", &time,&power);
					counter++;
					time_index++;
					//printf("%ld\t%ld",time_index, power);
					//printf("\n");
					if(power > max_power_at_interval)
					{
						max_power_at_interval = power;
						
					}
				}
				//current_time = current_time + INTERVAL;
				time_power_store[time_power_index][0] = time_index;
				time_power_store[time_power_index][1] = max_power_at_interval;
				time_power_index++;
			}	
			for(int kk = 0; kk< time_power_index ; kk++)
			{
				fprintf(write_output,"%ld\t",time_power_store[kk][0]);
			}
			//fprintf(write_output,"%ld",time_power_store[time_power_index - 1][0]);
			fprintf(write_output,"\n");
			for(int kk = 0; kk< time_power_index; kk++)
			{
				fprintf(write_output,"%ld\t",time_power_store[kk][1]);
			}
			//fprintf(write_output,"%ld",time_power_store[time_power_index - 1][1]);
			fprintf(write_output,"\n\n");
			fclose(read_input);
			i++;
			prev_no_of_clk_cycle = no_of_clk_cycle;
		}	
		/*else
		{	
			i++;
			prev_no_of_clk_cycle = no_of_clk_cycle;
		}*/		
	}
	fclose(write_output);
}
int main(int argc, char *argv [ ])
{
	int TAM_WIDTH_MAX = atoi (argv[1]);
	int NO_OF_MODULE = atoi (argv [2]);
	INTERVAL = atoi (argv[3]);
	for (int ii = 1 ; ii <= NO_OF_MODULE; ii++)
	{
		power_value_calculator (ii, TAM_WIDTH_MAX);	
	}
}

