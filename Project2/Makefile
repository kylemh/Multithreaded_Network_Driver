#CFLAGS=-W -Wall -pedantic -g
CFLAGS=-W -Wall -pedantic
OBJECTS=BoundedBuffer.o diagnostics.o fakeapplications.o \
        freepacketdescriptorstore.o generic_queue.o \
        networkdevice.o networkdriver.o packetdescriptor.o \
        packetdescriptorcreator.o testharness.o

mydemo: $(OBJECTS)
	gcc -o mydemo $(CFLAGS) $(OBJECTS) -lpthread

clean:
	rm -f networkdriver.o mydemo

networkdriver.o: networkdriver.c
