CC = gcc
CFLAGS = -W -Wall
LDFLAGS = -g -o
SOURCE1 = usps_v1.c p1fxns.h p1fxns.c
SOURCE2 = usps_v1.c p1fxns.h p1fxns.c
SOURCE3 = usps_v1.c p1fxns.h p1fxns.c
#SOURCE4 = usps_v1.c p1fxns.h p1fxns.c
PROGS = 1 2 3

all: $(PROGS)

1: usps_v1.o p1fxns.o
	cc $(LDFLAGS) usps_v1 usps_v1.o p1fxns.o

2: usps_v2.o p1fxns.o
	cc $(LDFLAGS) usps_v2 usps_v2.o p1fxns.o

3: usps_v3.o p1fxns.o
	cc $(LDFLAGS) usps_v3 usps_v3.o p1fxns.o

#4: usps_v4.o p1fxns.o
#	cc $(LDFLAGS) usps_v4 usps_v4.o p1fxns.o

usps_v1.o: $(SOURCE1)
usps_v2.o: $(SOURCE2)
usps_v3.o: $(SOURCE3)
#usps_v4.o: $(SOURCE4)
p1fxns.o: p1fxns.h p1fxns.c

clean:
	rm -f *~ *.o usps_v1 usps_v2 usps_v3 #usps_v4