#ifndef __RDP_H__
#define  __RDP_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sys/time.h>

#include "arraycode.h"

/**	Erasure code RDP. C++ version
  	@author WUXURUI
*/
using namespace std;
class rdp: public arraycode
{

private:
	int k;  					// n=k+m, k is the number of data disks 
	const int m = 2;			// m is the number of parity disks
	int n;						// n is the number of all disks
    int p_num;   				// a prime number
	int w; 						// the size of each strip
    int data_length;			// the original size of data in each disk
    int stripe_unit_size;  		// the size of data in each disk for encoding
    char** idata;  				// original data
    char** data;   				// temple data for encoding and decoding
	int xor_count = 0; 			// the account of xors
	double exec_time;			// the total time to execute the project
	struct timeval tvs, tve;	// the start-time and end-time of the project

public:
	/**	RDP class */    	
	rdp()
    {
    	k = 4;
		n = k + m;
    	p_num = 5;
		w = p_num - 1;
		data_length = 1024;
	 	stripe_unit_size = data_length;
		cout << "data_length is: " << data_length << endl
			 << "stripe_unit_size is: " << stripe_unit_size << endl << endl;

		idata = (char**)malloc(sizeof(char*) * n);
		for(int i = 0; i < n; i++) {
			idata[i] = (char*) malloc(sizeof(char) * stripe_unit_size);
			memset(idata[i],0,stripe_unit_size);
		}

		data = (char**)malloc(sizeof(char*) * n);
		for(int i = 0; i < n; i++) {
			data[i] = (char*) malloc(sizeof(char) * stripe_unit_size);
			memset(data[i],0,stripe_unit_size);
		}
	}
	
    rdp(int disk, int prime, int filesize)
	{
		k = disk;
		n = k + m;
		p_num = prime;
		w = p_num - 1;
		data_length = filesize/disk + ((filesize%disk > 0)?1:0); 
		stripe_unit_size = data_length; 
		cout<<"data_length is: "<<data_length<<endl<<"stripe_unit_size is: "<<stripe_unit_size<<endl<<endl;

		idata = (char**)malloc(sizeof(char*) * n);
		for(int i = 0; i < n; i++) {
			idata[i] = (char*) malloc(sizeof(char) * stripe_unit_size);
			memset(idata[i],0,stripe_unit_size);
		}
		
		data = (char**)malloc(sizeof(char*) * n);
		for(int i = 0; i < n; i++) {
			data[i] = (char*) malloc(sizeof(char) * stripe_unit_size);
			memset(data[i],0,stripe_unit_size);
		}
	}
	
	~rdp()
	{
		for(int i = 0; i < n; i++)  free(idata[i]);
		free(idata);
		for(int i = 0; i < n; i++)  free(data[i]);
		free(data);
	}

	/** rdp encoding main function. */
	void encoding()
	{
		//outputOrigin();
		cout<<"Encoding..."<<endl;		
		
		//gettimeofday(&tvs, NULL); //---------------------------
		rdp_encoding_r();
		//gettimeofday(&tve, NULL); 
		gettimeofday(&tvs, NULL); 
		cout<<"xor_num is :"<<xor_count<<endl;
		rdp_encoding_d();
		cout<<"xor_num is :"<<xor_count<<endl;
		gettimeofday(&tve, NULL); //---------------------------

		for(int i=0; i<2; i++) {
			string chunk_dir = "store/node" + to_string(k+i);
			write_file(chunk_dir, idata[k+i], stripe_unit_size);
		}		
		cout<<endl<<"After encoding..."<<endl;
		//outputOrigin();
		//outputData();
	}

	/** Encoding for row parity. */
	void rdp_encoding_r()
	{
		cout<<endl<<"Encoding row parity..."<<endl;	
		int packet_size = stripe_unit_size/w;
		for(int i = 0; i < k; i++) {                                             
			for(int off = 0; off < packet_size; off++)  {                                      
				for(int j = 0; j < w; j++) {
					idata[k][off*w+j] ^= idata[i][off*w+j];
					xor_count++;
					//cout<<"idata["<<k<<"]["<<j*packet_size+off<<"] ^= idata["<<i
					//	<<"]["<<j*packet_size+off<<"]:  "<<idata[k][j*packet_size+off]<<endl;
				}
			}
		}
	}

	/** Encoding for diagonal parity. */
	void rdp_encoding_d()
	{
		cout<<endl<<"Encoding diagonal parity..."<<endl;	
		int diag = 0;
		int packet_size = stripe_unit_size /w;

		for(int i = 0; i < k; i++) {                                             
			diag = i;
			for(int j = 0; j < w; j++) {
				if(diag < w)  {                                      
					for(int off = 0; off < packet_size; off++)  {
						idata[k+1][diag*packet_size+off] ^= idata[i][j*packet_size+off];
						xor_count++;
						//cout<<"idata["<<k+1<<"]["<<diag*packet_size+off<<"] ^= idata["<<i
						//	<<"]["<<j*packet_size+off<<"]:  "<<idata[k+1][diag*packet_size+off]<<endl;
					}
					diag++;
				}else {
					diag = 0;
				}
			}
		}

		for (int j = 1; j < w; j++) {
			for (int off = 0; off < packet_size; off++) {
				idata[k+1][(j-1)*packet_size+off] ^= idata[k][j*packet_size+off];
				xor_count++;
				//cout<<"idata["<<k+1<<"]["<<(j-1)*packet_size+off<<"] ^= idata["<<k
				//	<<"]["<<j*packet_size+off<<"]:  "<<idata[k+1][(j-1)*packet_size+off]<<endl;
			}
		}
	}

	/**	rdp decoding main function. */
	void decoding(int* err, int length)
	{
		cout<<endl<<"Decoding..."<<endl;
		int errNum = 0, errCount = length;
		int one, two;

		for(int i=0; i<k+2; i++) {
			string chunk_dir = "store/node" + to_string(i);
			read_file(chunk_dir, idata[i], stripe_unit_size);
		}

		one = err[0];
		memset(idata[one], 0, stripe_unit_size*sizeof(char));
		if(length == 2)  {
			two = err[1];
			memset(idata[two], 0, stripe_unit_size*sizeof(char));
		} 
		for(int i = 0; i < errCount; i++)  {
			if(err[i] >= 0 && err[i] < k)  {
				errNum ++;
			}	
		}
		
		for(int i = 0; i < k+2; i++) {
			memcpy(data[i], idata[i], stripe_unit_size * sizeof(char));
		}  

		if(errNum == 0){												              // there are two parity nodes broken
			if(one == k)  rdp_encoding_r();
			if(one == k+1 || two == k+1)  rdp_encoding_d();
		} 

		gettimeofday(&tvs, NULL); //---------------------------

		if(errNum == 1) {
			if(two == k) {                                                        // there are only one data node and the row parity node broken
				rdp_decoding_rd(data, k, stripe_unit_size, w, one, two);
				memcpy(idata[one], data[one], stripe_unit_size* sizeof(char));
				memcpy(idata[two], data[two], stripe_unit_size* sizeof(char));
			} else {
				rdp_decoding_r(data, k, stripe_unit_size, w, one);                // there is only one data node broken
				memcpy(idata[one], data[one], stripe_unit_size* sizeof(char));
				if(two == k+1)	{												  // there are only one data node and the diagonal parity node broken
					rdp_encoding_d();
				}
			}
		}  
		
		if(errNum == 2) {                                                      		  // there are two data nodes broken
			rdp_decoding_rd(data, k, stripe_unit_size, w, one, two);
			memcpy(idata[one], data[one], stripe_unit_size* sizeof(char));
			memcpy(idata[two], data[two], stripe_unit_size* sizeof(char));
		} 
		
		gettimeofday(&tve, NULL); //---------------------------

		cout<<endl<<"After decoding..."<<endl;
		//outputOrigin();
		//outputData();
	}

	/** rdp_decoding_r(data, k, stripe_unit_size, w, one);
      	rdp decoding according row parity.
      	@param data
      	@param k
      	@param stripe_unit_size
      	@param w
      	@param one
    */
    void rdp_decoding_r(char** data, int k, int stripe_unit_size, int w, int one)
    {
        int packet_size = stripe_unit_size / w;

        for(int i=0; i<(k+1); i++) {
            if(i!=one) {
				for (int p = 0; p < w; p++) {
					for (int off = 0; off < packet_size; off++) {
						data[one][p*packet_size+off] ^= data[i][p*packet_size+off];
						xor_count++;
						//cout<<"data["<<one<<"]["<<p*packet_size+off<<"] ^= data["<<i
						//	<<"]["<<p*packet_size+off<<"]:  "<<data[one][p*packet_size+off]<<endl;
					}
				}
			}
        }
    }
	
	/**	rdp_decoding_rd(data, k, stripe_unit_size, w, one, two);
	 	rdp decoding according row and diagonal parity.
	 	@param data
	 	@param k
	 	@param stripe_unit_size
	 	@param w
	 	@param one/two:  the first/second error disk in data
	*/
	void rdp_decoding_rd(char** data, int k, int stripe_unit_size, int w, int one, int two)
	{
		int packet_size = stripe_unit_size / w;
		
		for(int diag = mod(one-1, p_num); diag < k; diag = mod(diag-two+one, p_num)) {
			for(int off=0; off < packet_size; off++) {
				for(int i=0; i < k+2; i++) {
					if(i!=two && mod(diag-i, p_num) < k) {
						data[two][packet_size*mod(diag-two, p_num)+off] ^= data[i][packet_size*mod(diag-i, p_num)+off];
						xor_count++;
						//cout<<"data["<<two<<"]["<<packet_size*mod(diag-two, p_num)+off<<"] ^= data["<<i
						//	<<"]["<<packet_size*mod(diag-i, p_num)+off<<"]:  "<<data[two][packet_size*mod(diag-two, p_num)+off]<<endl;
					}
				}
				for(int i=0; i < k+1; i++) {
					if(i!=one)  {
						data[one][packet_size*mod(diag-two, p_num)+off] ^= data[i][packet_size*mod(diag-two, p_num)+off];
						xor_count++;
						//cout<<"data["<<one<<"]["<<packet_size*mod(diag-two, p_num)+off<<"] ^= data["<<i
						//	<<"]["<<packet_size*mod(diag-two, p_num)+off<<"]:  "<<data[one][packet_size*mod(diag-two, p_num)+off]<<endl;
					}
				}
			}
		}

		for(int diag = mod(two+k, p_num); diag < k; diag = mod(diag-one+two, p_num)) {
			for(int off = 0; off < packet_size; off++) {
				for(int i = 0; i < k+2; i++) {
					if(i!=one && mod(diag-i, p_num) <= k-1)  {
						data[one][packet_size*mod(diag-one, p_num)+off] ^= data[i][packet_size*mod(diag-i, p_num)+off];
						xor_count++;
						//cout<<"data["<<one<<"]["<<packet_size*mod(diag-one, p_num)+off<<"] ^= data["<<i
						//	<<"]["<<packet_size*mod(diag-i, p_num)+off<<"]:  "<<data[one][packet_size*mod(diag-one, p_num)+off]<<endl;
					}
				}
				for(int i = 0; i < k+1; i++) {
					if(i!=two)  {
						data[two][packet_size*mod(diag-one, p_num)+off] ^= data[i][packet_size*mod(diag-one, p_num)+off];
						xor_count++;
						//cout<<"data["<<two<<"]["<<packet_size*mod(diag-one, p_num)+off<<"] ^= data["<<i
						//	<<"]["<<packet_size*mod(diag-one, p_num)+off<<"]:  "<<data[two][packet_size*mod(diag-one, p_num)+off]<<endl;
					}				
				}
			}
		}	
	}
    
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
			string chunk_dir = "store/node" + to_string(i);
			write_file(chunk_dir, idata[i], stripe_unit_size);
		}
		return 0;
	}
	
	/**	mod(int, int)
		mod(a,b) <==> a mod b
	*/
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


	//	for testing and debug.
    void outputData()
    {
        cout<<"The parity data:"<<endl;
		for(int i = 0; i < 2; i++) {
            cout<<"odata["<<i<<"]:  ";
			for(int j = 0; j < stripe_unit_size; j++)  {
				if(idata[k+i][j] == 0) 
					cout<<" ";
				cout<<idata[k+i][j];
			}
			cout<<endl;
        }
		cout<<endl;
    }

    void outputOrigin()
    {
		cout<<"The origin data:"<<endl;
        for(int i = 0; i < k; i++) {
            cout<<"idata["<<i<<"]:  ";
			for(int j = 0; j < stripe_unit_size; j++)  {
				if(idata[i][j] == 0) 
					cout<<" ";
				cout<<idata[i][j];
			}
			cout<<endl;
        }
		cout<<endl;
    }

	void return_xors()
	{
		cout<<endl<<"num of xors is: "<<xor_count<<endl;
	}

	void return_time()
	{
		exec_time = (tve.tv_sec-tvs.tv_sec) + (tve.tv_usec-tvs.tv_usec)/1000000.0;
		cout<<"total time is: "<<exec_time<<"s"<<endl;
	}

	string showme()
	{
		return "RDP";
	}

};

#endif
