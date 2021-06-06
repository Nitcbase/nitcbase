#pragma once

int insert(char relname[ATTR_SIZE], int nAttrs, char record[][ATTR_SIZE]);
int select(char srcrel[ATTR_SIZE], char targetrel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strval[ATTR_SIZE]);
int project(char srcrel[ATTR_SIZE], char targetrel[ATTR_SIZE], int tar_nAttrs, char tar_attrs[][ATTR_SIZE]);
int join(char srcrel1[ATTR_SIZE], char srcrel2[ATTR_SIZE], char targetrel[ATTR_SIZE], char attr1[ATTR_SIZE], char attr2[ATTR_SIZE]);
