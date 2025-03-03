#ifndef __UART__
#define __UART__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

typedef enum
{
    UART1,
    UART2,
    UART3,
    UART4,
    UART5,
    UART_MAX_NUM,
}UART_MEMBER;

#define UART1_ROUTE "/dev/ttyS1"
#define UART2_ROUTE "/dev/ttyS2"
#define UART3_ROUTE "/dev/ttyS3"
#define UART4_ROUTE "/dev/ttyS4"
#define UART5_ROUTE "/dev/ttyS5"

#define FATAL(fmt, ...) printf("[\x1b[31m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define DEBUG(fmt, ...) printf("[\x1b[35m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define UART_TEMP_BUFFER_SIZE 1024

#define UART_THREAD_NUM 2

typedef struct 
{
    int uart_fd;
    sem_t sem;
}uart_t;

void start_uart_thread();

#endif
