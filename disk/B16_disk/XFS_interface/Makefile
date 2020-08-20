CC = g++
CFLAGS = -w

default: xfs-interface

xfs-interface: *.cpp *.h
	$(CC) $(CFLAGS) interface_new.cpp -o xfs-interface 

clean:
	$(RM) xfs-interface *.o *.out disk block_allocation_map attribute_catalog relation_catalog
 

