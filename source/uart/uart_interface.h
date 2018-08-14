#ifndef URAT_INTER_H
#define URAT_INTER_H


//interface
//void uart_send_path(char*,int);
void uart_send_charList(char* ,int ,int);
void uart_read_charFour();
void uart_read_charList(char* read_data,int data_len);
int UART0_Open(int fd,char* port);
int UART0_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);

#endif

