//#include "gyro.h"
//it's gyro branch
#include "uart_interface.h"
#include <stdio.h>
#include "string.h"
float d_x=0;
float d_y=0;
float d_z=0;
float d_angle=0;

float v_x1=0;
float v_y1=0;
float v_z1=0;

float v_x0=0;
float v_y0=0;
float v_z0=0;

double gravityX = 0;
double gravityY = 0;
double gravityZ = 0;

float a1[3]={0};
float w1[3]={0};
float angle1[3]={0};
float others[3]={0};

float a0[3]={0};
float w0[3]={0};
float angle0[3]={0};

int fd;
int UART0_Open(int fd,char* port);
int UART0_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);
#define FALSE  -1
#define TRUE   0
//note:when close the UART, nothing will be stored!!!
/*******************************************************************************
 The purpose of the calibration routine is to obtain the value of the reference threshold.
 It consists on a 1024 samples average in no-movement condition.
********************************************************************************/
void calibration(void)
{
     unsigned int count1;
     char temp_buf[11]={0};
     count1 = 0;
     do{
     // ADC_GetAllAxis();
     // sstatex = sstatex + Sample_X; // Accumulate Samples
     // sstatey = sstatey + Sample_Y;
     // count1++;
        uart_read_gyro(fd,temp_buf);
        if((unsigned char)temp_buf[0]==0x55)
        {
            if((unsigned char)temp_buf [1] == 0x51)
            {
            //case 0x51:
                // for(int i=0;i<3;i++)
                // {
                //     a0[i] = a1[i];
                // }
                a1[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*16;
                a1[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*16;
                a1[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*16;
                //T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                printf("%d\n",count1 );
                printf("ax = %f\n", a1[0]);
                printf("ay = %f\n", a1[1]);
                printf("az = %f\n", a1[2]);
                printf("\n\n");
                //break;
                count1++;
                gravityX = gravityX + a1[0];
                gravityY = gravityY + a1[1];
                gravityZ = gravityZ + a1[2];
            }
        }


     }while(count1!= 128 ); // 1024 times
     gravityX=gravityX / 128; // division between 1024
     gravityY=gravityY / 128;
     gravityZ=gravityZ / 128;
     printf("gx = %f\n", gravityX);
     printf("gy = %f\n", gravityY);
     printf("gz = %f\n", gravityZ);
     //while(1);
}
/*****************************************************************************************/
int main(int argc, char const *argv[])
{
    /* code */
    unsigned char time0[8]={0};
    unsigned char time1[8]={0};
    unsigned int Dtime = 0;


    float T =0;
    char temp_buf[11]={0};
    unsigned char counter = 0;

    char addr[] = "/dev/ttyUSB0";
    int err;                           //返回调用函数的状态
    fd = UART0_Open(fd,addr); //打开串口，返回文件描述符
    do
    {
        err = UART0_Init(fd,9600,0,8,1,'N');
        printf("Set Port Exactly!\n");
    }while(FALSE == err || FALSE == fd);

    calibration();

    while(1)
    {
        // while(counter != 1)
        // {
            uart_read_gyro(fd,temp_buf);
            if((unsigned char)temp_buf[0]==0x55)
            {
                switch((unsigned char)temp_buf [1])
                {
                case 0x50:
                    memcpy(&time0,&time1,8);
                    memcpy(&time1,&temp_buf[2],8);
                    Dtime = (unsigned int)\
                            (time1[3]-time0[3])*3600*1000 +\
                            (time1[4]-time0[4])*60*1000 +\
                            (time1[5]-time0[5])*1000 +\
                            (time1[6]-time0[6]) + (time1[7]-time0[7])*256;

                    // for(int i=0;i<8;i++)
                    // {
                    //     printf("%d,", time1[i]);
                    // }
                    printf("\nms = %d\n\n",Dtime);
                    break;

                case 0x51:
                    // for(int i=0;i<3;i++)
                    // {
                    //     a0[i] = a1[i];
                    // }
                    a1[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*16;
                    a1[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*16;
                    a1[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*16;
                    T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                    counter++;
                    printf("ax = %f\n", a1[0]);
                    printf("ay = %f\n", a1[1]);
                    printf("az = %f\n", a1[2]);
                    printf("\n\n");

                    a1[0] = a1[0] - (float)gravityX;//ax
                    a1[1] = a1[1] - (float)gravityY;//ay
                    a1[2] = a1[2] - (float)gravityZ;//az
                    printf("after g offset\nax = %f\n", a1[0]);
                    printf("ay = %f\n", a1[1]);
                    printf("az = %f\n", a1[2]);
                    printf("\n\n");
                    for(int i=0;i<3;i++)
                    {
                        if(a1[i] <0.0015 && a1[i] >-0.0015)
                            a1[i] = 0;
                        a0[i] = a1[i];
                    }
                    v_x0 = v_x1;
                    v_y0 = v_y1;
                    v_z0 = v_z1;
                    //100hz = 0.1s = 100ms  0.1 * 10 = 1
                    v_x1 = v_x0 + (a1[0] + a0[0])/2;
                    d_x = d_x + (v_x1 + v_x0)/2;

                    v_y1 = v_y0 + (a1[1] + a0[1])/2;
                    d_y = d_y + (v_y1 + v_y0)/2;

                    v_z1 = v_z0 + (a1[2] + a0[2])/2;
                    d_z = d_z +  (v_z1 + v_z0)/2;

                    break;
                case 0x52:
                    // for(int i=0;i<3;i++)
                    // {
                    //     w0[i] = w1[i];
                    // }
                    w1[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*2000;
                    w1[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*2000;
                    w1[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*2000;
                    T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                    // printf("w1 = %f\n", w1[0]);
                    // printf("w2 = %f\n", w1[1]);
                    // printf("w3 = %f\n", w1[2]);
                    // printf("\n\n");
                    break;
                case 0x53:
                    // for(int i=0;i<3;i++)
                    // {
                    //     angle0[i] = angle1[i];
                    // }
                    angle1[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*180;
                    angle1[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*180;
                    angle1[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*180;
                    T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                    // printf("angle1 = %f\n", angle1[0]);
                    // printf("angle2 = %f\n", angle1[1]);
                    // printf("angle3 = %f\n", angle1[2]);
                    // printf("\n\n");
                    break;
                // default:
                //     others[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*180;
                //     others[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*180;
                //     others[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*180;
                //     T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                //     printf("others1 = %f\n", others[0]);
                //     printf("others2 = %f\n", others[1]);
                //     printf("others3 = %f\n", others[2]);
                //     printf("\n\n");
                }
            }
            printf("\n\n");
        // }

        // counter = 0;
        // a1[0] = a1[0] / 64;//ax
        // a1[1] = a1[1] / 64;//ay
        // a1[2] = a1[2] / 64;//az
        // printf("after average\nax = %f\n", a1[0]);
        // printf("ay = %f\n", a1[1]);
        // printf("az = %f\n", a1[2]);

        // a1[0] = a1[0] - (float)gravityX;//ax
        // a1[1] = a1[1] - (float)gravityY;//ay
        // a1[2] = a1[2] - (float)gravityZ;//az
        // printf("after g offset\nax = %f\n", a1[0]);
        // printf("ay = %f\n", a1[1]);
        // printf("az = %f\n", a1[2]);
        // printf("\n\n");


        //whether Discrimination window applied?
        // if ((accelerationx[1] <=3)&&(accelerationx[1] >= -3)) //Discrimination window applied
        //    {accelerationx[1] = 0;} // to the X axis acceleration
        // if ((accelerationy[1] <=3)&&(accelerationy[1] >= -3))
        //    {accelerationy[1] = 0;}





        d_angle = angle1[2] - angle0[2];

        printf("v_x1 = %f,v_y1 = %f, v_z1 = %f\n",v_x1,v_y1,v_z1);
        printf("v_x0 = %f,v_y0 = %f,v_z0 = %f\n", v_x0,v_y0,v_z0);
        printf("d_x = %f, d_y = %f, d_z = %f,d_angle = %f\n\n",d_x,d_y,d_z,d_angle );





    }

    return 0;
}