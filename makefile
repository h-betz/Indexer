all: index.c libsl.a
	gcc -w -o index index.c libsl.a
    
    libsl.a: sorted-list.o
	ar r libsl.a sorted-list.o
        
    sorted-list.o: sorted-list.c sorted-list.h
	gcc -c sorted-list.c

clean:
	rm *.o
	rm libsl.a
	rm ./index
