#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO; /* the index number of the watchpoint */
	struct watchpoint *next;
    char* expression;
    uint32_t old_value;
	/* TODO: Add more members if necessary */


} WP;

WP *get_WP_list();
void add_WP(const char* expression, uint32_t old_value);
int del_WP(int n);
int check_WP();
#endif
