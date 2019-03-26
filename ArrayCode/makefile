CXX = g++ -std=c++11
SRCS = rdp_test.cpp eardp_test.cpp rs_test.cpp star_test.cpp arraycode.cpp
OBJS = $(SRCS:.cpp = .o)

all: rdp_test eardp_test rs_test star_test arraycode

rdp_test:rdp_test.o
	$(CXX) -o rdp_test rdp_test.o
rdp_test.o:rdp_test.cpp arraycode.h rdp.h
	$(CXX) -c rdp_test.cpp

eardp_test:eardp_test.o
	$(CXX) -o eardp_test eardp_test.o
eardp_test.o:eardp_test.cpp arraycode.h eardp.h
	$(CXX) -c eardp_test.cpp

rs_test:rs_test.o
	$(CXX) -o rs_test rs_test.o
rs_test.o:rs_test.cpp arraycode.h rs.h
	$(CXX) -c rs_test.cpp

star_test:star_test.o
	$(CXX) -o star_test star_test.o
star_test.o:star_test.cpp arraycode.h star.h
	$(CXX) -c star_test.cpp

arraycode:arraycode.o
	$(CXX) -o arraycode arraycode.o
arraycode.o:arraycode.cpp arraycode.h rdp.h eardp.h star.h rs.h
	$(CXX) -c arraycode.cpp

.PHONY : clean
clean:
	rm rdp_test.o rdp_test eardp_test.o eardp_test rs_test.o rs_test star_test.o star_test arraycode.o arraycode