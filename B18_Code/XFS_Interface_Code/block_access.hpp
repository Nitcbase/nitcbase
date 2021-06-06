#pragma once

recId linear_search(relId relid, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId *prev_recid);
int ba_search(relId relid, union Attribute *record, char attrName[ATTR_SIZE], union Attribute attrval, int op, recId *prev_recid);
int ba_insert(int relid, union Attribute *rec);