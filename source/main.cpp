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
extern Mat Image;
using namespace std;

int queen_path(bool flag)
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
    printf("%d,%d,%d\n",p_x,p_y,p_theta);
    // char p_char[6];
    // p_char[0] = p_x>>8;
    // p_char[1] = p_x;
    // p_char[2] = p_y>>8;
    // p_char[3] = p_y;
    // p_char[4] = p_theta>>8;
    // p_char[5] = p_theta;

    // printf("%d,",p_char[0]*256+(unsigned char)p_char[1] );
    // printf("%d,",p_char[2]*256+(unsigned char)p_char[3] );
    // printf("%d\n",p_char[4]*256+(unsigned char)p_char[5] );
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



    // for(int i=0;i<6;i++)
    // {
    //     printf("%d\n",(unsigned char)p_char[i] );
    // }
    //uart_send_charList(p_char,6);
}

void toChar_send_path()
{
    // char* send_data = (char*) malloc(1+2*2 * NUM * sizeof(char));

    // send_data[0] = 2*NUM;

    // for(int i = 0; i <=NUM-1; i++)
    // {
        // send_data[4*i+1] = routeB[i].x;
        // send_data[4*i+2] = routeB[i].y;
        // send_data[4*i+3] = routeA[i].x;
        // send_data[4*i+4] = routeA[i].y;
        // printf("(%d,%d)\n",send_data[4*i+1],send_data[4*i+2] );
        // printf("(%d,%d)\n",send_data[4*i+3],send_data[4*i+4] );
    // }

    // uart_send_charList(send_data,4*NUM+1);

    char fourByteData[4] = {0,0,2*NUM,'N'};
    uart_send_charList(fourByteData,4);


    for(int i=0;i<4;i++)
    {
        fourByteData[i]=0;
    }
    for(int i=0;i<NUM;i++)
    {
        fourByteData[2] = routeB[i].x;
        fourByteData[3] = 'X';
        uart_send_charList(fourByteData,4);

        fourByteData[2] = routeB[i].y;
        fourByteData[3] = 'Y';
        uart_send_charList(fourByteData,4);

        fourByteData[2] = routeA[i].x;
        fourByteData[3] = 'X';
        uart_send_charList(fourByteData,4);

        fourByteData[2] = routeA[i].y;
        fourByteData[3] = 'Y';
        uart_send_charList(fourByteData,4);


    }

}


int main(int argc, char const *argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s 1/0(input or not) 1/0(for cam) \n",argv[0]);
        return 0;
    }
    bool flag1,flag2;
    if(strcmp(argv[1],"0") == 1)
        flag1 = 1;
    else
        flag1 = 0;

    queen_path(flag1);

    printf("\n\n\n\npath send by uart:\n");
    toChar_send_path();

    uart_read_charFour();

    char fourByteDataX[4] = {0,0,100,'x'};
    char fourByteDataY[4] = {0,0,100,'y'};

    for(int i=0;i<15;i++)
    {
        fourByteDataX[2] -= 5*i;
        fourByteDataY[2] += 5*i;
        uart_send_charList(fourByteDataX,4);
        uart_send_charList(fourByteDataY,4);
        sleep(2);
    }

    while(1);




    printf("\n\n\n\nlocation send by uart:\n");
    ROIData position_car;
    if(strcmp(argv[2],"0") == 1)
    {
        VideoCapture cap;
        cap.open(1);

        int work_time = 50;
        while(work_time--)
        {
            cap>>Image;
            position_car = maindoCamera();
            if(position_car.center.x == 0)
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
            position_car = maindoPicture();
            toChar_send_position(position_car);
        }
}





