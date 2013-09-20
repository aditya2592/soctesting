#ifndef soctest_H
#define soctest_H
 
class core;

class tam
{
	int tamid;
	int width;
	int test_time;
	int  ass_cores[50];
	int count;
	public :
	tam()
	{
		count=0;
		for(int i=0; i<50; i++)
		{
			ass_cores[i] = -1;
		}
	}
	void store_tam(int id, int w)
	{
		tamid = id;
		width = w;
		test_time = 0;
	}
	int get_width()
	{
		return width;
	}
	int get_time()
	{
		return test_time;
	}
	void update_time(int time)
	{
		test_time = test_time + time;
	}
	int get_tam_id()
	{
		return tamid;
	}
	/*void put_core(core * temp, int time)
	{
		ass_cores[count] =  temp;
		test_time = test_time + time;
		count++;
	}*/
	void put_core(int time, int coreid)
	{
		test_time = test_time + time;
		ass_cores[count] = coreid;
		count++;
	}
	int * get_cores()
	{

		return ass_cores;
	}
};
class core
{
	int coreid;
	int is_assg;
	tam mytam;
	public:
	core()
	{
		is_assg = 0;
	}
	void store_id(int id)
	{
		coreid = id;
		is_assg = 0;
	}
	void update_assg(tam t)
	{
		is_assg = 1;
		mytam = t;

	}
	int is_assgn()
	{
		if(is_assg)
			return 1;
		else
			return 0;
	}
	tam get_core_tam()
	{
		return mytam;
	}
	int get_id()
	{
		return coreid;
	}
};
class times
{
	core cr;
	tam ta;
	int test_time;
	public:
	void store_test(core c, tam t, int time)
	{
		cr = c;
		ta = t;
		test_time = time;
	}
	int get_time()
	{
		return test_time;
	}
};

//Below here added on Tuesday//
#endif
