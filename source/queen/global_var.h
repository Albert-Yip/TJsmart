#ifndef Data_H
#define Data_H

typedef struct
{
    int x;
    int y;
    //struct chessNode* match;
}chessNode;

#define MAX_SOLUTION_AMOUNT 10
//global var
extern chessNode chess[MAX_SOLUTION_AMOUNT][8],solution[MAX_SOLUTION_AMOUNT][8];//chess: the original positions; solution: the chosen solution
extern int max_coincide;//the sum of chess in the exact place
extern int NUM;
extern chessNode routeA[MAX_SOLUTION_AMOUNT][8];// will be malloc in main
extern chessNode routeB[MAX_SOLUTION_AMOUNT][8];
extern int amount_coincidence;
extern int min_route_index;

#endif