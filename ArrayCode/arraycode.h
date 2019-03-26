#ifndef __ARRAYCODE_H__
#define __ARRAYCODE_H__

#include <string.h>

using namespace std;

class arraycode {

private:
    int k;  					// n=k+m, k is the number of data disks 
	int m;						// m is the number of parity disks
	int n;						// n is the number of all disks
	int w; 						// the size of each strip
    int data_length;			// the original size of data in each disk
    int stripe_unit_size;  		// the size of data in each disk for encoding
    char** idata;  				// original data
    char** data;   				// temple data for encoding and decoding
	int xor_count = 0; 			// the account of xors
	double exec_time;			// the total time to execute the project
	struct timeval tvs, tve;	// the start-time and end-time of the project

public:	
	virtual string showme() = 0;
	virtual void encoding() = 0;
	virtual void decoding(int* err, int length) = 0;
	virtual int setData(string srcdir, string filename) = 0; 
	virtual void return_xors() = 0;
	virtual void return_time() = 0;
/*
	int setData(string srcdir, string filename)
	{	
		string src = srcdir + '/' + filename;
		FILE *fp = fopen(src.c_str(), "rb");
		if(fp == NULL) {
			cout << "file is null !!!" << endl;
			return -1;
		}

		fseek(fp, 0, SEEK_SET);
		for(int j = 0; j < stripe_unit_size; j++)  {
			for(int i = 0; i < k; i++)  {
				if(mod(j+1, w) == 0) {
					idata[i][j] == 0;
				} else {
					fscanf(fp, "%c", &idata[i][j]);
				}
			}
		}
		fclose(fp);

		for(int i=0; i<k; i++) {
			cout<<"SCSCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"<<endl;
			string chunk_dir = "store/node" + to_string(i);
			write_file(chunk_dir, idata[i], stripe_unit_size);
		}
		return 0;
	}
	
	//	mod(int, int)
	//	mod(a,b) <==> a mod b
	int mod(int a, int b)
	{
		if(a%b <0)  
			return (a%b+b);
		else  
			return (a%b);
	}

	// 	read chunks from files 
	void read_file(string path, char *data, size_t size)
	{
		FILE *fp = fopen(path.c_str(), "rb");
		if (fp == NULL) {
			cout<<path<<" is null !!!"<<endl;
		}
		if (fread(data, 1, size, fp) != size) {
			cout<<path<<" read is error !!!"<<endl;
		}
		fclose(fp);
	}
	
	// 	write chunks to files
	void write_file(string dst, char *data, size_t size)
	{
		FILE *fp = fopen(dst.c_str(), "wb");
		if (fp == NULL) {
			cout<<"file is null !!!"<<endl;
		}
		if (fwrite(data, 1, size, fp) != size) {
			cout<<"file write is error !!!"<<endl;
		}
		fclose(fp);
	}
*/

};

#endif
