//
// Created by Gokul Sreekumar on 06/06/21.
//

#ifndef B18_CODE_DISK_H
#define B18_CODE_DISK_H


class Disk {
public:
	Disk() {}
	~Disk() {}
	static int readBlock(unsigned char *block, int blockNum);
	static int writeBlock(unsigned char *block, int blockNum);
};


#endif //B18_CODE_DISK_H
