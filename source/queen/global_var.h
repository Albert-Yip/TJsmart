#ifndef Data_H
#define Data_H

typedef struct
{
    int x;
    int y;
    //struct chessNode* match;
}chessNode;

//global var
extern chessNode chess[8],solution[8];//chess: the original positions; solution: the chosen solution
extern int max_coincide;//the sum of chess in the exact place
extern int NUM;
extern chessNode* routeA;// will be malloc in main
extern chessNode* routeB;

#endif