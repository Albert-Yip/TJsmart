#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "find_path.h"
#include "readfile_match.h"
#include "global_var.h"
#include <unistd.h>     /*Unix 标准函数定义*/

#include "uart_interface.h"
#include "cam_interface.h"


extern int num;
extern Mat Image,Image2,Image3,Image4;
using namespace std;
extern int show_flag;
int sum_X=0, sum_Y=0;
int queen_path(int flag);
void toChar_send_position(ROIData position_car);
void toChar_send_path();
void send_wall_1();
void send_chess_n(int n);
void one_time_move();

int main(int argc, char const *argv[])
{
    int n = 0;
    if(argc != 3)
    {
        
        printf("Usage: %s 3/2/1/0(chess or wall/queen test/input/not) 1/0(for cam) \n",argv[0]);
        return 0;
    }
    if(strcmp(argv[1],"3") == 0)//for chess or wall, move one time
    {
    	one_time_move();
    }
    else if(strcmp(argv[1],"2") == 0)//for test =2
    {

        printf("please input n (how many step you wanna go): \n");
        scanf("%d",&n);
        send_chess_n(n);
    }
    else
    {
        queen_path(strcmp(argv[1],"0"));
        //while(1);
        printf("\n\n\n\npath send by uart:\n");
        toChar_send_path();
        n = 2 * NUM;
    }


    //while(1);

    printf("\n\nnow waiting for the 5604:  and the right answer is %d,%d\n",sum_X,sum_Y);
    uart_read_charFour();

    // char fourByteDataX[4] = {0,0,100,'x'};
    // char fourByteDataY[4] = {0,0,100,'y'};

    // for(int i=0;i<15;i++)
    // {
    //     fourByteDataX[2] -= 5*i;
    //     fourByteDataY[2] += 5*i;
    //     uart_send_charList(fourByteDataX,4);
    //     uart_send_charList(fourByteDataY,4);
    //     sleep(2);
    // }

    //while(1);


    show_flag = 1;

    printf("\n\n\n\nlocation send by uart:\n");
    ROIData position_car;
    if(strcmp(argv[2],"0") == 1)//=1
    {
        printf("ready to go!\n");
        VideoCapture cap;
        VideoCapture cap2;
        // VideoCapture cap3;
        // VideoCapture cap4;
        //while(1);
        while(!cap.open(1));
        while(!cap2.open(2));
        // while(!cap3.open(3));//L
        // while(!cap4.open(4));//R
        int work_time = n*300;
        while(work_time--)
        {
            cap>>Image;
            cap2>>Image2;
            // cap3>>Image3;
            // cap4>>Image4;

            while (Image.empty())
            {
		        cap.open(1); cap>>Image;printf("cam open retrying");

            }
            while (Image2.empty())
            {
		        cap2.open(2); cap2>>Image2;printf("cam2 open retrying");
                
            }
            // while (Image3.empty())
            // {
            //     cap3.open(3); cap>>Image3;printf("cam3 open retrying");

            // }
            // while (Image4.empty())
            // {
            //     cap4.open(4); cap2>>Image4;printf("cam4 open retrying");

            // }
            position_car = maindoCamera();
            if(position_car.center.x == 0 && position_car.center.y == 0 && position_car.theta == 0)
             {
                printf("nothing!\n");
            }
            else
                toChar_send_position(position_car);
        }
        //printf("%d\n",num );
    }
    else
    {
        //printf("ready to go???????????\n");
        position_car = maindoPicture();
        toChar_send_position(position_car);
    }
    //printf("???????????\n");
}


void one_time_move()
{
	int cw=0;
    printf("move chess or wall? 0->chess/1->wall: \n");
    scanf("%d",&cw);
    if(!cw)//chess
    {
    	send_chess_n(1);
    }
    else//wall
    {
    	send_wall_1();
    }

}

void send_wall_1()
{
	int *position;
	char heading;
	printf("please input the 2 positions for wall and its heading(awsd) : \n");
    for(int i=0;i<2;i++)
    {
        scanf("%d",position+i);
    }
	scanf(" %c",&heading);
	char fourByteData[4] = {0,0,0,'W'};
    uart_send_charList(fourByteData,4);
    //printf("the heading is %c\n",heading);
    for(int i=0;i<2;i++)
    {

        fourByteData[2] = (*(position+i) - 1) % 8 + 1;//original X
        if(heading == 'a')
    	{
    		fourByteData[2] = fourByteData[2] * 2 - 1 ;
    	}
    	else if (heading == 'd')
    	{
    		fourByteData[2] = fourByteData[2] * 2 + 1 ;//X = (X+-0.5)*2
    	}
        else
            fourByteData[2] = fourByteData[2] * 2;

        fourByteData[3] = 'X';
        uart_send_charList(fourByteData,4);

        sum_X += fourByteData[2];

        fourByteData[2] = (*(position+i) - 1) / 8 + 1;//original Y
        if(heading == 'w')
    	{
    		fourByteData[2] = fourByteData[2] *2 - 1 ;
    	}
    	else if (heading == 's')
    	{
    		fourByteData[2] = fourByteData[2] *2 + 1 ;//Y = (Y+-0.5)*2
    	}
        else
            fourByteData[2] = fourByteData[2] * 2;
        fourByteData[3] = 'Y';
        uart_send_charList(fourByteData,4);


        //sum_X += routeA[i].x;
        sum_Y += fourByteData[2];
        //sum_Y += routeA[i].y;

    }
    //the end point
    // char end_fourByteData[4] = {0,0,0,'E'};
    // int end_X = (*(position+n-1) - 1) % 8 + 1;
    // int end_Y = (*(position+n-1) - 1) / 8 + 1;
    // if(abs(end_X - 4.5) > abs(end_Y - 4.5))
    // {
    //     //x is the chosen ending
    //     if(end_X - 4.5 > 0)
    //         end_fourByteData[3]='R';//ascii: 82
    //     else
    //         end_fourByteData[3]='L';//ascii: 76
    // }
    // else
    // {
    //     //y is the chosen ending
    //     if(end_Y - 4.5 > 0)
    //         end_fourByteData[3]='B';//ascii: 66
    //     else
    //         end_fourByteData[3]='F';//ascii: 70
    // }
    char end_fourByteData[4] = {0,0,0,'E'};
    end_fourByteData[3]='B';//ascii: 66
    uart_send_charList(end_fourByteData,4);
}

void send_chess_n(int n)
{
	int *position;
	position = (int*) malloc(n * sizeof(int));
    printf("please input positions for %d times : \n",n);
    for(int i=0;i<n;i++)
    {
        scanf("%d",position+i);
    }
    char fourByteData[4] = {0,0,n,'N'};
    uart_send_charList(fourByteData,4);
    for(int i=0;i<n;i++)
    {

        fourByteData[2] = (*(position+i) - 1) % 8 + 1;
        fourByteData[3] = 'X';
        uart_send_charList(fourByteData,4);

        sum_X += fourByteData[2];

        fourByteData[2] = (*(position+i) - 1) / 8 + 1;
        fourByteData[3] = 'Y';
        uart_send_charList(fourByteData,4);


        //sum_X += routeA[i].x;
        sum_Y += fourByteData[2];
        //sum_Y += routeA[i].y;

    }
    //the end point
    char end_fourByteData[4] = {0,0,0,'E'};
    int end_X = (*(position+n-1) - 1) % 8 + 1;
    int end_Y = (*(position+n-1) - 1) / 8 + 1;
    if(abs(end_X - 4.5) > abs(end_Y - 4.5))
    {
        //x is the chosen ending
        if(end_X - 4.5 > 0)
            end_fourByteData[3]='R';//ascii: 82
        else
            end_fourByteData[3]='L';//ascii: 76
    }
    else
    {
        //y is the chosen ending
        if(end_Y - 4.5 > 0)
            end_fourByteData[3]='B';//ascii: 66
        else
            end_fourByteData[3]='F';//ascii: 70
    }
    uart_send_charList(end_fourByteData,4);
}

int queen_path(int flag)
{
    clock_t start, end;
    start = clock();

    input_position(flag);

    read_and_match();
    NUM = 8 - max_coincide;
    routeA = (chessNode*) malloc(NUM * sizeof(chessNode));
    routeB = (chessNode*) malloc(NUM * sizeof(chessNode));
    path();

    end = clock();
    printf("total time = %f\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}

void toChar_send_position(ROIData position_car)
{
    char fourByteData[4] = {0,0,0,0};

    int p_x = (int)position_car.center.x;
    int p_y = (int)position_car.center.y;
    int p_theta = (int)(position_car.theta*10);
    printf("(%d,%d,%d)\n",p_x,p_y,p_theta);

    fourByteData[1] = p_x>>8;
    fourByteData[2] = p_x;
    fourByteData[3] = 'x';
    uart_send_charList(fourByteData,4);

    fourByteData[1] = p_y>>8;
    fourByteData[2] = p_y;
    fourByteData[3] = 'y';
    uart_send_charList(fourByteData,4);

    fourByteData[1] = p_theta>>8;
    fourByteData[2] = p_theta;
    fourByteData[3] = 'a';
    uart_send_charList(fourByteData,4);

}

void toChar_send_path()
{

    char fourByteData[4] = {0,0,2*NUM,'N'};
    uart_send_charList(fourByteData,4);


    for(int i=0;i<4;i++)
    {
        fourByteData[i]=0;
    }
    for(int i=0;i<NUM;i++)
    {
        fourByteData[2] = routeB[i].x;//the chess
        fourByteData[3] = 'X';
        uart_send_charList(fourByteData,4);

        fourByteData[2] = routeB[i].y;
        fourByteData[3] = 'Y';
        uart_send_charList(fourByteData,4);

        fourByteData[2] = routeA[i].x;//the position
        fourByteData[3] = 'X';
        uart_send_charList(fourByteData,4);
        
        fourByteData[2] = routeA[i].y;
        fourByteData[3] = 'Y';
        uart_send_charList(fourByteData,4);

        sum_X += routeB[i].x;
        sum_X += routeA[i].x;
        sum_Y += routeB[i].y;
        sum_Y += routeA[i].y;
    }

    //the end point
    char end_fourByteData[4] = {0,0,0,'E'};

    if(abs(routeA[NUM-1].x - 4.5) > abs(routeA[NUM-1].y - 4.5))
    {
        //x is the chosen ending
        if(routeA[NUM-1].x - 4.5 > 0)
            end_fourByteData[3]='R';//ascii: 82
        else
            end_fourByteData[3]='L';//ascii: 76
    }
    else
    {
        //y is the chosen ending
        if(routeA[NUM-1].y - 4.5 > 0)
            end_fourByteData[3]='B';//ascii: 66
        else
            end_fourByteData[3]='F';//ascii: 70
    }
    uart_send_charList(end_fourByteData,4);



}





