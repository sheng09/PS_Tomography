#include "Eqsta.h"
#include <iostream>
int main(int argc, char const *argv[])
{
	//float a=86400.0;
	//float b=0.2;

	//Time t2(2000,1,1,0,0,0.00);
	//Time t1(2000,1,1,0,1,0.00);

	Time t2(2009,1,1,9,44,58.79);
	Time t1(2009,1,1,9,45,36.16);
	Time t3(1970,1,1,0,0,0.10);
	double  dt = Dt(t2,t1);
	//double ldt= Dt(t1,t2);
	//long  lt = (long) (dt);
	std::cout<< dt<< '\n';
	std::cout<< t1.absoluteTime()<<'\n';
	std::cout<< t3.absoluteTime()<<'\n';
	//std::cout<< ldt<< '\n';
	//std::cout<< lt<< '\n';
	return 0;
}