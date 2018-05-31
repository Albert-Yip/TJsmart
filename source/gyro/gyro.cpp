//#include "gyro.h"
//it's gyro branch
#include "uart_interface.h"
#include <stdio.h>
#include "string.h"
//note:when close the UART, nothing will be stored!!!
int main(int argc, char const *argv[])
{
    /* code */
    unsigned char time0[8]={0};
    unsigned char time1[8]={0};
    unsigned int Dtime = 0;

    float a1[3]={0};
    float w1[3]={0};
    float angle1[3]={0};
    float others[3]={0};

    float a0[3]={0};
    float w0[3]={0};
    float angle0[3]={0};
    float T =0;
    char temp_buf[11]={0};

    while(1)
    {
        uart_read_gyro(temp_buf);
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
                        (time1[6]-time0[6])*256 + (time1[7]-time0[7]);

                for(int i=0;i<8;i++)
                {
                    printf("%d,", time1[i]);
                }
                printf("\nms = %d\n\n",Dtime);
                break;

            case 0x51:
                for(int i=0;i<3;i++)
                {
                    a0[i] = a1[i];
                }
                a1[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*16;
                a1[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*16;
                a1[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*16;
                T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;

                printf("ax = %f\n", a1[0]);
                printf("ay = %f\n", a1[1]);
                printf("az = %f\n", a1[2]);
                printf("\n\n");
                break;
            case 0x52:
                for(int i=0;i<3;i++)
                {
                    w0[i] = w1[i];
                }
                w1[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*2000;
                w1[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*2000;
                w1[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*2000;
                T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                printf("w1 = %f\n", w1[0]);
                printf("w2 = %f\n", w1[1]);
                printf("w3 = %f\n", w1[2]);
                printf("\n\n");
                break;
            case 0x53:
                for(int i=0;i<3;i++)
                {
                    angle0[i] = angle1[i];
                }
                angle1[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*180;
                angle1[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*180;
                angle1[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*180;
                T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                printf("angle1 = %f\n", angle1[0]);
                printf("angle2 = %f\n", angle1[1]);
                printf("angle3 = %f\n", angle1[2]);
                printf("\n\n");
                break;
            default:
                others[0] = (short(temp_buf [3]<<8| temp_buf [2]))/32768.0*180;
                others[1] = (short(temp_buf [5]<<8| temp_buf [4]))/32768.0*180;
                others[2] = (short(temp_buf [7]<<8| temp_buf [6]))/32768.0*180;
                T = (short(temp_buf [9]<<8| temp_buf [8]))/340.0+36.25;
                printf("others1 = %f\n", others[0]);
                printf("others2 = %f\n", others[1]);
                printf("others3 = %f\n", others[2]);
                printf("\n\n");
            }
        }
        printf("\n\n");

        int d_x=0;
        int d_y=0;
        int d_angle=0;
        int v_x1=0;int v_x0=0;
        int v_y1=0;int v_y0=0;

        v_x1 = v_x0 + Dtime * (a1[0] + a0[0])/2;
        d_x = d_x + Dtime * (v_x1 + v_x0)/2;

        v_y1 = v_y0 + Dtime * (a1[1] + a0[1])/2;
        d_y = d_y + Dtime * (v_y1 + v_y0)/2;

        d_angle = angle1[2] - angle0[2];

        printf("d_x = %d, d_y = %d, d_angle = %d\n\n",d_x,d_y,d_angle );





    }

    return 0;
}