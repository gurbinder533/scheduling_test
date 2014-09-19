#include <iostream>
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
#define CHILD_STACK 4096

void *stack;
volatile bool start;

int hash_func(void* arg){
	while(!start)
	{
		std::cout << "waiting for all I am :" << getpid()<<"\n"; 
	}
	int a = 0;
	//std::cout << "pid : " << getpid()<<"\n";
	/* calculate hash128 */
	/*char buff[4096];
	file.read(buff, 4096);
 	const uint128 h = CityHash128(buff, 4096);	
	*/
	//std::cout<<"THRD ID :" << getpid() << " hashed Val : "<< h.first << "\n";
	std::cout <<" I am done\n";
	exit(1);
}
int main()
{	
	std::ifstream file;
	start = false;
	file.open("dev/urandom");
		
	int num_children = 5;
	size_t len1 = 44;
	std::vector<void*> stack_vec;
	std::vector<int> TID_vec;
	for(int i = 0; i < num_children; ++i) {
		
		void *stack = ::operator new(CHILD_STACK);
		if(!stack) {
		      std::cout <<"The stack failed\n";		
		      exit(0);
		}
		stack_vec.push_back(stack); //for deallocation.

		int thrd_id = clone(&hash_func, (char*)stack+CHILD_STACK, CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND, 0);
		TID_vec.push_back(thrd_id);
		std::cout<<"Thread id : " << thrd_id <<"\n";
	}
	
	start = true;
	int wpid, result, noresult, status;
	std::vector<bool> flag_exit(num_children);
	std::cout << "Parent waiting for children to die\n";
	std::vector<int> wpid_vec(num_children);
//	sleep(1000);
/*	while(1) {
			
	//	std::cout << "in while wait \n";
		for(int i = 0 ; i < TID_vec.size(); ++i) {
			wpid_vec[i] = waitpid(TID_vec[i], &result, WNOHANG|__WCLONE);
			if(wpid_vec[i] == TID_vec[i]) {
				flag_exit[i] = true;	
			}
			else flag_exit[i] = false;
		}
		
		bool done = true;
		for(auto i = 0 ; i < flag_exit.size(); ++i) {
			if(!(flag_exit[i])) {
				done = false;
				break;
			}	
		}

		if(done) 
			break;
	}
*/
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
