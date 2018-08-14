#include "readfile_match.h"
#include "vector"
//global var
using namespace std;

chessNode chess[MAX_SOLUTION_AMOUNT][8],solution[MAX_SOLUTION_AMOUNT][8];//chess: the original positions; solution: the chosen solution
int max_coincide = 0;//the sum of chess in the exact place
int amount_coincidence = 0;
// vector<vector<chessNode>> chess, solution;


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
    vector<int> max_Index;

    for (int i = 0; i < 92; i++)//第i个解
    {
        for (int n = 0; n < 8; n++)//原布局的第n个棋子
        {
            if (all_Solution[i][chess[0][n].y] == chess[0][n].x)
                coincide_n[i]++;
        }
        if (coincide_n[i] > max_coincide)
        {
            max_coincide = coincide_n[i];
            max_Index.clear();
            max_Index.push_back(i);
        }
        else if(coincide_n[i] == max_coincide)
        {
            max_Index.push_back(i);
        }

    }
    amount_coincidence = max_Index.size();
    printf("Amount of solution with the same coincident place: %d\n",amount_coincidence);

    //update
    for(int n=0;n<amount_coincidence;n++)
    {   
        if(n<MAX_SOLUTION_AMOUNT)
        {
            for(int i=0;i<8;i++)
            {
                solution[n][i].y = i+1;
                solution[n][i].x=all_Solution[max_Index[n]][i+1];
            }            
        }

    }
    // for(int i=0;i<8;i++)
    // {
    //     solution[i].y = i+1;
    //     solution[i].x=all_Solution[max_Index][i+1];
    // }

    update_chess();

    /**********************************
     *test and show the result
     **********************************/

    for(int n=0;n<amount_coincidence;n++)
    {   
        if(n<MAX_SOLUTION_AMOUNT)
        {

            printf("\nchess originial place : \n");
            for(int i=0;i<8;i++)
            {
                int num = chess[n][i].x + (chess[n][i].y-1)*8;
                printf("%d ",num);
            }
            printf("\nchess target place : \n");
            for(int i=0;i<8;i++)
            {
                int num = solution[n][i].x + (solution[n][i].y-1)*8;
                printf("%d ",num);
            }

            printf("\n\nmax_Index = %d\n", max_Index[n]);
            printf("coincide = %d\n", max_coincide);


            //show the result!
            printf("     ");
            for (int i = 1; i <= 8; i++)
                printf("%d ", i);
            printf("\n");
            int flag_Display = 0;
            for (int y = 1; y <= 8; y++)//y是行号
            {
                printf("  %d  ", y);
                for (int x = 1; x <= 8; x++)//x是列号
                {
                    for(int i=8 - max_coincide;i<8;i++)
                    {
                        if(x == chess[n][i].x && y == chess[n][i].y)//in the same place
                        {
                            printf("@ ");
                            flag_Display = 1;
                            break;
                        }
                    }
                    if(!flag_Display)// have not displayed
                    {
                        for(int i=0;i<8-max_coincide;i++)
                        {
                            if(x == chess[n][i].x && y == chess[n][i].y)//chess position
                            {
                                printf("C ");
                                flag_Display = 1;
                                break;
                            }
                        }

                        if(!flag_Display)// have not displayed
                        {
                            if (all_Solution[max_Index[n]][y] == x)
                            {
                                printf("Q ");
                            }
                            else
                            {
                                printf(". ");
                            }
                        }
                    }
                    flag_Display = 0;


                }
                printf("\n");
            }
        }
    }
    // printf("read and match finished\n");

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
            for(int j=0;j<MAX_SOLUTION_AMOUNT;j++)
            {
                chess[j][i].x = (position - 1) % 8 + 1;
                chess[j][i].y = (position - 1) / 8 + 1;
                //chess[i].match = NULL;
            }



        }
    }
    else
    {
        // int position[8] = {12,19,56,44,23,5,33,39};
        int position[8] = {12,45,62,25,29,36,49,22};
        for (int i = 0; i < 8; i++)
        {
            for(int j=0;j<MAX_SOLUTION_AMOUNT;j++)
            {          
                chess[j][i].x = (position[i] - 1) % 8 + 1;
                chess[j][i].y = (position[i] - 1) / 8 + 1;
                //chess[i].match = NULL;
            }
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
    for(int n=0;n<amount_coincidence;n++)
    {   
        if(n<MAX_SOLUTION_AMOUNT)
        {
            for(int i=7;i>=0;i--)
            {
                for(int j=7;j>=0;j--)
                {

                    if(chess[n][i].x == solution[n][j].x && chess[n][i].y == solution[n][j].y)
                    {
                        place_behind(chess[n],i);
                        place_behind(solution[n],j);

                        //a++;
                        break;
                    }
                }
            }
        }
    }
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
