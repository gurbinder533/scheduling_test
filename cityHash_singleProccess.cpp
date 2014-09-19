#include <iostream>
#include <fstream>
#include <cstdlib>
#include "city.h"
#include "time.h"

int main()
{	
	std::ifstream file("dev/urandom");
	char buff[4096];// = new char[4096];
	int len = 4096;
	file.read(buff, 4096);
	int i = 1000000;
	time_t end = time(NULL) + 5;
	int count = 0;
	while(time(NULL) <= end) {
        	const uint128 hashed = CityHash128(buff,len);
		++count;
	}
	std::cout << "count : " << count <<"\n";
//	std::cout<<"hashed : "<< hashed.first << "\n";
	file.close();
}
