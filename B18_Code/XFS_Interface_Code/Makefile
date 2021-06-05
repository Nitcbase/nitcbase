CC = g++
#CFLAGS = -w
RM = rm

c_flags = -c

source_files = interface_stack.cpp disksimulator.cpp
output_files = interface_stack.o disksimulator.o
out_files = *.out

default: xfs-interface

xfs-interface: *.cpp *.h
	$(CC) ${c_flags} ${source_files}
	$(CC) ${output_files} -o xfs-interface 

clean:
	$(RM) xfs-interface ${output_files} ${out_files} disk block_allocation_map attribute_catalog relation_catalog