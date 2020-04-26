#include "eardp.h"
#include "sys/time.h"

int main(int argc, char **argv) 
{
	double totalTime;
	struct timeval stvs, stve;

    if (argc < 2)  {
		cout<<"Usage: ./eardp [encode|decode|repair] filename (repair node no.) ..."<<endl; 
		exit(1);
	}
	int mode = !strncmp(argv[1], "encode", 7)? 0 :        // 0 = encode 
              (!strncmp(argv[1], "repair", 7)? 1 : 2);    // 1 = rapair 
	
	cout<<"Starting..."<<endl;
	string src = "src/";
	src += argv[2];
	FILE *fp = fopen(src.c_str(), "rb");
	if(fp == NULL) {
		cout << "file is null !!!" << endl;
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	size_t filesize = ftell(fp);
	cout << "the size of file is: " << filesize << endl;
	
	eardp *eardpItem = new eardp(4, 5, filesize);
	if(eardpItem->setData("src", argv[2])) {
		return -1;
	}
	
	gettimeofday(&stvs, NULL); //---------------------------

	if(mode == 0) {
		eardpItem->encoding();
	}
	if(mode == 1) {
		if(argc == 3) {            
			cout<<"No Error data need be recovery! "<<endl;
			return -1;
		}
		if(argc > 5){
			cout<<"Error NUM is too larger! It should be [0, 1, 2] "<<endl;
			return -1;
		}

		int err[2] = {0};
		int length = 0;
		err[0] = atoi(argv[3]);
		length = 1;
		if(argc == 5)  {
			err[1] = atoi(argv[4]);
			length = 2 ;
		}
		//rdpItem->encoding();
		eardpItem->decoding(err, length);
	}

	gettimeofday(&stve, NULL);
	totalTime = (stve.tv_sec-stvs.tv_sec) + (stve.tv_usec-stvs.tv_usec)/1000000.0;
	cout<<"sys total time is: "<<totalTime<<"s"<<endl;
	eardpItem->return_time();
	eardpItem->return_xors();
	
	return 0;
}
