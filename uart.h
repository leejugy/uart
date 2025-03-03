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
    UART1,        /* uart1번 인덱스 */
    UART2,        /* uart2번 인덱스 */
    UART3,        /* uart3번 인덱스 */
    UART4,        /* uart4번 인덱스 */
    UART5,        /* uart5번 인덱스 */
    UART_MAX_NUM, /* 최대 길이 */
} UART_MEMBER;

#define UART1_ROUTE "/dev/ttyS1" // uart1 경로
#define UART2_ROUTE "/dev/ttyS2" // uart2 경로
#define UART3_ROUTE "/dev/ttyS3" // uart3 경로
#define UART4_ROUTE "/dev/ttyS4" // uart4 경로
#define UART5_ROUTE "/dev/ttyS5" // uart5 경로

#define FATAL(fmt, ...) printf("[\x1b[31m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__) // 오류 정보 프린트 함수
#define DEBUG(fmt, ...) printf("[\x1b[35m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__) // 디버그 정보 프린트 함수
#define UART_TEMP_BUFFER_SIZE 1024                                                          // 임시 버퍼

#define UART_THREAD_NUM 2 // 현재 uart 스레드 수

typedef struct
{
    int uart_fd; /* uart 파일 디스크립터 */
    sem_t sem;   /* 세마포어 */
} uart_t;

/**
 * @brief 스레드 시작 함수
 *
 */
void start_uart_thread();

#endif
