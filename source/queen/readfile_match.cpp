#include "readfile_match.h"

//global var

chessNode chess[8],solution[8];//chess: the original positions; solution: the chosen solution
int max_coincide = 0;//the sum of chess in the exact place


void read_and_match()
{
    /**********************************
     *read 92 queen solutions from file
     **********************************/
    FILE *fp;
    int all_Solution[92][9] = { 0 };
    if((fp=fopen("../source/queen/queen.txt","r"))==NULL)
    {
        printf("can't open file\n");
        exit(1);
    }

    int temp=0;
    for(int i=0;i<92;i++)
    {
        for(int y=1;y<=9;y++)
        {

            temp=fgetc(fp);
            if(temp!='\n')
                all_Solution[i][y]=temp;
        }
    }



    //input
    //input_position();



    /**********************************
     *match the best solution and update solution[]
     **********************************/
    int coincide_n[92] = { 0 };
    int max_Index = -1;
    for (int i = 0; i < 92; i++)//第i个解
    {
        for (int n = 0; n < 8; n++)//原布局的第n个棋子
        {
            if (all_Solution[i][chess[n].y] == chess[n].x)
                coincide_n[i]++;
        }
        if (coincide_n[i] > max_coincide)
        {
            max_coincide = coincide_n[i];
            max_Index = i;
        }

    }

    //update
    for(int i=0;i<8;i++)
    {
        solution[i].y = i+1;
        solution[i].x=all_Solution[max_Index][i+1];
    }

    update_chess();

    /**********************************
     *test and show the result
     **********************************/
    printf("\nchess originial place : \n");
    for(int i=0;i<8;i++)
    {
        int num = chess[i].x + (chess[i].y-1)*8;
        printf("%d ",num);
    }
    printf("\nchess target place : \n");
    for(int i=0;i<8;i++)
    {
        int num = solution[i].x + (solution[i].y-1)*8;
        printf("%d ",num);
    }

    printf("\n\nmax_Index = %d\n", max_Index);
    printf("coincide = %d\n", max_coincide);
    printf("     ");
    for (int i = 1; i <= 8; i++)
        printf("%d ", i);
    printf("\n");
    for (int y = 1; y <= 8; y++)//y是行号
    {
        printf("  %d  ", y);
        for (int x = 1; x <= 8; x++)//x是列号
        {
            if (all_Solution[max_Index][y] == x)
            {
                printf("Q ");
            }
            else
            {
                printf(". ");
            }
        }
        printf("\n");
    }


}

/**********************************
 * input the position through keyboard
 **********************************/
void input_position(bool flag)
{
    if(flag)
    {
        printf("please input the chess positions on board:\n");
        for (int i = 0; i < 8; i++)
        {

            //scanf("%d%d", &chess[i].x, &chess[i].y);
            int position = 0;
            scanf("%d",&position);
            chess[i].x = (position - 1) % 8 + 1;
            chess[i].y = (position - 1) / 8 + 1;
            //chess[i].match = NULL;


        }
    }
    else
    {
        int position[8] = {12,19,56,44,23,5,33,39};
        for (int i = 0; i < 8; i++)
        {
            chess[i].x = (position[i] - 1) % 8 + 1;
            chess[i].y = (position[i] - 1) / 8 + 1;
            //chess[i].match = NULL;
        }
    }
}


/**********************************
 * put the chess in the exact position behind
 **********************************/
void update_chess()
{
    chessNode temp;
    //int a = 0;
    for(int i=7;i>=0;i--)
    {
        for(int j=7;j>=0;j--)
        {

            if(chess[i].x == solution[j].x && chess[i].y == solution[j].y)
            {
                place_behind(chess,i);
                place_behind(solution,j);

                //a++;
                break;
            }
        }
    }
    // if(a!=max_coincide)
    // {
    //     printf("update failed!!!!!\na = %d",a);
    // }
}

void place_behind(chessNode a[], int i)
{
    if(i == 7)
        return;
    chessNode temp={0,0};
    temp = a[i];
    for(int j=i+1;j<8;j++)
    {
        a[j-1] = a[j];
    }
    a[7] = temp;
}

// int main(void)
// {
//     /* code */
//     read_and_match();

//     return 0;
// }
