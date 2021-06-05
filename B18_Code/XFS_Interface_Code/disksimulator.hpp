#pragma once

#include <iostream>
#include "../define/constants.h"
#include "disk_structure.h"

void createdisk();

void formatdisk();

int readblock(void *buffer, int block_no);

int writeblock(int block_no, void *buffer);