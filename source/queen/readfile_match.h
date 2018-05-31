#ifndef READFILE_MATCH_H
#define READFILE_MATCH_H

#include <stdio.h>
#include <stdlib.h>
#include "global_var.h"
//interface
void read_and_match();
void input_position(bool flag);

//internal use function
void place_behind(chessNode a[], int i);
void update_chess();
void show_result();
#endif
