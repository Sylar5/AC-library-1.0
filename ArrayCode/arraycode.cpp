#include "rdp.h"
#include "eardp.h"
#include "rs.h"
#include "star.h"
#include "arraycode.h"

#include "sys/time.h"

int main(int argc, char **argv) 
{
	double total_time;
	struct timeval stvs, stve;
    arraycode* code_item;
    int k = 0, m = 0 ,w = 0;

    if (argc < 6)  {
		cout << endl;
        cout << "Usaged: ./arraycode [rdp|eardp|rs|star] k m w [encode|decode|repair...] filename (repair node no.) ..." << endl; 
        cout << endl;
		exit(1);
	}

    int code_type =  ! strncmp(argv[1], "rdp", 4)   ? 0 :
                    (!(strncmp(argv[1], "eardp", 6) ? 1 :
                    (!(strncmp(argv[1], "rs", 3)    ? 2 :
                    (!(strncmp(argv[1], "star", 5)  ? 3 : 4))))));

    k = atoi(argv[2]);
    m = atoi(argv[3]);
    w = atoi(argv[4]);

    int mode = !strncmp(argv[5], "encode", 7)? 0 :        // 0 = encode 
              (!strncmp(argv[5], "repair", 7)? 1 : 2);    // 1 = rapair 
	
	cout<<"Starting..."<<endl;
	string src = "src/";
	src += argv[6];
	FILE *fp = fopen(src.c_str(), "rb");
	if(fp == NULL) {
		cout << "file is null !!!" << endl;
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	size_t filesize = ftell(fp);
	cout << "the size of file is: " << filesize << endl << endl;
	
	switch(code_type) {
        case(0):
            code_item = new rdp(k, w, filesize);
            break;
        case(1):
            code_item = new eardp(k, w, filesize);
            break;
    /*  case(2):
            code_item = new rs();
            break;
        case(3):
            code_item = new star();
            break;
    */
        default:
            cout << "the code type is not defined!" << endl;
    }

	if(code_item->setData("src", argv[6])) {
		return -1;
	}
	
	gettimeofday(&stvs, NULL); //---------------------------

	if(mode == 0) {
		code_item->encoding();
	}
	if(mode == 1) {
		if(argc == 7) {            
			cout<<"No Error data need be recovery! "<<endl;
			return -1;
		}
		if(argc > 9){
			cout<<"Error NUM is too larger! It should be [0, 1, 2] "<<endl;
			return -1;
		}

		int err[2] = {0};
		int length = 0;
		err[0] = atoi(argv[7]);
		length = 1;
		if(argc == 8)  {
			err[1] = atoi(argv[8]);
			length = 2 ;
		}
		code_item->decoding(err, length);
	}

	gettimeofday(&stve, NULL);
	total_time = (stve.tv_sec-stvs.tv_sec) + (stve.tv_usec-stvs.tv_usec)/1000000.0;
	cout<<"sys total time is: "<<total_time<<"s"<<endl;
	code_item->return_time();
	code_item->return_xors();
	
	return 0;
}
