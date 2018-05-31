#ifndef URAT_INTER_H
#define URAT_INTER_H


//interface
//void uart_send_path(char*,int);
void uart_send_charList(char* ,int );
void uart_read_charFour();
void uart_read_charList(char* read_data,int data_len);
void uart_read_gyro(char* read_data);

#endif

