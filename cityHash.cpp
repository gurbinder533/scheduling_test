#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include "city.h"
#include "time.h"
#include <sched.h>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include "backgroundTask.c"

#define CHILD_STACK 8192
//#define no 0;
void *stack;
volatile bool start_hash;

std::ifstream file;

#ifdef with_time 
#else 
	#define with_count 1
#endif

int hash_func(void* arg){
	while(!start_hash)
	{
	//	std::cout << "waiting for all I am :" << getpid()<<"\n"; 
	}
	int a = 0;
	//std::cout << "pid : " << getpid()<<"\n";
	/* calculate hash128 */
	char buff[4096];
	file.read(buff, 4096);
	time_t end = time(NULL) + 5;
	int count = 0;
#ifdef with_count
	while(time(NULL) <= end) {
 		const uint128 h = CityHash128(buff, 4096);	
		++count;
	}


	std::cout<<"THRD ID :" << getpid() << " Count : "<< count << "\n";
#endif

#ifdef with_time
	size_t number_of_hashes = 3000000;
	//time_t start = time(NULL);
    auto start = std::chrono::high_resolution_clock::now();
	for(int i = 0; i < number_of_hashes; ++i) {
 		const uint128 h = CityHash128(buff, 4096);	
	}
	//time_t end1 = time(NULL);
    auto end1 = std::chrono::high_resolution_clock::now();

	//std::cout << "Thrd id: " << getpid() << "  Time Taken : " << (end1 - start) << "\n";
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start).count();
    std::cout<<"Thrd id: " <<getpid() << " Time Taken : " << duration << " ms\n";
#endif
	//std::cout <<" I am done\n";
	exit(1);
}
int main(int argc, char *argv[])
{	
    // to measure the throughput os scheduling.	
	//time_t total_start = time(NULL);
    auto total_start = std::chrono::high_resolution_clock::now();

	/**
	 * set CPU affinity 
	 **/
	//XXX Is it inhereted by children ??
/*
#ifdef parent_aff
	cpu_set_t mask;
	CPU_ZERO(&mask); // initialize mask to ZERO 
	CPU_SET(1, &mask); // Use processor 0
	if(sched_setaffinity(0, sizeof(mask), &mask)) {
		perror("sched_setaffinity"); // Always check for errors
	}
#endif
*/
	start_hash = false;
	file.open("dev/urandom");
	int bg_processes = 0 ;		
	int num_children = 2;
	if(argc >=2 ) {
		std::istringstream ss(argv[1]);
		if(!(ss >> num_children)) 
			std::cerr << "Oops.. Invalid number or something..."<< argv[1] << "\n";
		if(argc >=3) {
		    std::istringstream ss2(argv[2]); // background process.
		    if(!(ss2 >> bg_processes))
			    std::cerr << "Oops.. Invalid number or something..."<< argv[2] << "\n";
	        }
	}
	size_t len1 = 44;
	std::vector<void*> stack_vec;
	std::vector<int> TID_vec;
	//std::cout << "num_children : " << num_children << "  bg_proc: " <<bg_processes<<"\n";
	int processor_id = 0;
	for(int i = 0; i < num_children; ++i) {
		
		void *stack = ::operator new(CHILD_STACK);
		if(!stack) {
		      std::cout <<"The stack failed\n";		
		      exit(0);
		}
		stack_vec.push_back(stack); //for deallocation.

		int thrd_id = clone(&hash_func, (char*)stack+CHILD_STACK, CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND, 0);
        if(thrd_id == -1 ) {
            perror("Can't clone"); 
            break;
        }
		TID_vec.push_back(thrd_id);

		// pin each process to different physical core.
    #ifdef cpu_aff
		cpu_set_t mask;
		CPU_ZERO(&mask); // initialize mask to ZERO 
		CPU_SET(processor_id, &mask); // Use processor 0
		if(sched_setaffinity(thrd_id, sizeof(mask), &mask)) {
			perror("sched_setaffinity"); // Always check for errors
		}
        
		if(processor_id == 0)
			processor_id = 1;
		else 
			processor_id = 0;
        
    #endif
		//std::cout<<"Thread id : " << thrd_id <<"\n";
	}

	// start background processes 
	if(bg_processes > 0) {
	    start(bg_processes);
	}


    start_hash = true;
	int wpid, result, noresult, status;
	std::vector<bool> flag_exit(num_children);
	std::cout << "Parent waiting for children to die\n";
	std::vector<int> wpid_vec(num_children);

//	sleep(1000);
	int good, bad;
	while(1) {
		
		good = bad = 0;		
	//	std::cout << "in while wait \n";
		for(int i = 0 ; i < TID_vec.size(); ++i) {
			if((wpid_vec[i] = waitpid(TID_vec[i], &result, WNOHANG|__WCLONE)) == -1)
				++bad;	
			if(wpid_vec[i] == TID_vec[i]) {
				++good;	
			}
		}
		
		if( good == num_children || bad == num_children)
			break;
	}


	// Kill bg_processes 
	if(bg_processes > 0) {
	    stop();
	}
    
	//time_t total_end = time(NULL);
	//std::cout << "Total time : " << (total_end - total_start) << "s\n";
    auto total_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();
	std::cout << "Total time : " << duration << " ms\n";

	
/*	char buff[4096];
	int len = 4096;
	file.read(buff, 4096);
	int i = 1000000;
	time_t end = time(NULL) + 5;
	
	//while(time(NULL) <= end) {
        	const uint128 hashed = CityHash128(buff,len);
	//}
	std::cout<<"PARENT ID :" << getpid() << " hashed val : "<< hashed.first << "\n";
	
	if(waitpid(thrd_id,NULL, 0) == -1)
		std::cout<<"waitint\n";
	//	errExit("waitpid");
*/
	file.close();

/*	while( !stack_vec.empty()) {	
		auto ii = stack_vec.back();
		::operator delete(ii);
		stack_vec.pop_back();
	}
*/
}
