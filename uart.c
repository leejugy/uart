#include "uart.h"

uart_t ut[UART_MAX_NUM] = {
    0,
};

/**
 * @brief uart 구조체 초기화 하는 함수
 *
 * @param member 멤버를 선택하여 구조체 전역 변수 ut의 특정 인덱스 초기화
 * @return int 실패시 -1, 성공시 1
 */
static int init_uart_structure(UART_MEMBER member)
{
    int ret = 0;
    struct termios term = {
        0,
    };
    char *uart_route;

    switch (member) // /dev/ttyS* 의 device character file 경로 지정
    {
    case UART1:
        uart_route = UART1_ROUTE;
        break;

    case UART2:
        uart_route = UART2_ROUTE;
        break;

    case UART3:
        uart_route = UART3_ROUTE;
        break;

    case UART4:
        uart_route = UART4_ROUTE;
        break;

    case UART5:
        uart_route = UART5_ROUTE;
        break;

    default:
        break;
    }

    ret = sem_init(&ut[member].sem, 0, 1); // 세마포어 초기화
    if (ret < 0)
    {
        perror("fail to init semaphore");
        return -1;
    }

    ret = open(uart_route, O_RDWR | O_NOCTTY); // NOCCTY = 컨트롤 신호(ctrl + c) 무시, 읽기 쓰기 권한 부여
    if (ret < 0)
    {
        perror("fail to open uart");
        return -1;
    }

    ut[member].uart_fd = ret;

    term.c_cc[VTIME] = 0;                                    // 타임아웃 시간 0 (nonblocking)
    term.c_cc[VMIN] = 0;                                     // 최소 문자수 0 (nonblocking)
    term.c_cflag = B115200 | CREAD | CS8 | CLOCAL & ~CSTOPB; // 115200, 읽기 허용, 데이터 비트 8, 모뎀제어신호 무시, 스탑비트 1
    term.c_iflag = IGNPAR;                                   // 입력 패리티 무시

    ret = tcflush(ut[member].uart_fd, TCIOFLUSH); // 입출력 버퍼 모두 비우기
    if (ret < 0)
    {
        perror("fail to flush");
        return -1;
    }

    ret = tcsetattr(ut[member].uart_fd, TCSANOW, &term); // 지금 당장 속성 적용용
    if (ret < 0)
    {
        perror("fail to set attr");
        return -1;
    }

    return 1;
}

/**
 * @brief uart 초기화 하는 함수
 *
 * @return int 실패시 -1, 성공시 1
 */
static int init_uart()
{
    int ret = 0;

    ret = init_uart_structure(UART1);
    if (ret < 0)
    {
        return -1;
    }

    return 1;
}

/**
 * @brief uart 수신함수
 *
 * @param mem 사용할 uart 인덱스 번호
 * @param recv 수신될 버퍼
 * @param recv_size 수신 버퍼 크기
 * @return int 실패시 -1, 성공시 1
 */
static int uart_recv(UART_MEMBER mem, void *recv, size_t recv_size)
{
    int ret = 0;

    ret = read(ut[mem].uart_fd, recv, recv_size); // read 함수 써서 rx
    if (ret < 0)
    {
        perror("fail to read data");
        return ret;
    }

    return ret;
}

/**
 * @brief uart 전송 함수
 *
 * @param mem 사용할 uart 인덱스 번호
 * @param send 전송할 버퍼
 * @param send_size 전송송 버퍼 크기
 * @return int 실패시 -1, 성공시 1
 */
static int uart_send(UART_MEMBER mem, void *send, size_t send_size)
{
    int ret = 0;

    ret = write(ut[mem].uart_fd, send, send_size); // write 써서 tx
    if (ret < 0)
    {
        perror("fail to write data");
        return ret;
    }

    return ret;
}

/**
 * @brief 1번 수신 스레드
 *
 * @return int 실패시 -1, 성공시 1
 */
static void thread_uart1_rx()
{
    char uart_rx[UART_TEMP_BUFFER_SIZE] = {
        0,
    };
    int rx_size = 0;

    while (1)
    {
        memset(uart_rx, 0, sizeof(uart_rx));
        rx_size = uart_recv(UART1, uart_rx, sizeof(uart_rx));
        if (rx_size < 0)
        {
            FATAL("fail to recv");
        }

        else if (rx_size > 0)
        {
            DEBUG("recv : %s", uart_rx);
        }
    }
}

/**
 * @brief 1번 전송 스레드
 *
 * @return int 실패시 -1, 성공시 1
 */
static void thread_uart1_tx()
{
    char uart_tx[UART_TEMP_BUFFER_SIZE] = {
        0,
    };
    int tx_size = 0;
    int ret = 0;

    while (1)
    {
        memset(uart_tx, 0, sizeof(uart_tx));

        ret = scanf("%s", uart_tx);
        if (ret != 1)
        {
            FATAL("fail to scanf");
        }

        tx_size = uart_send(UART1, uart_tx, strlen(uart_tx));
        if (tx_size < 0)
        {
            FATAL("fail to recv");
        }
    }
}

void start_uart_thread()
{
    pthread_t tid[UART_THREAD_NUM] = {
        0,
    };
    void(*function_pointer[UART_THREAD_NUM]) = {&thread_uart1_rx, &thread_uart1_tx};
    uint8_t i = 0;
    int ret = 0;

    init_uart(); // 초기화

    for (i = 0; i < UART_THREAD_NUM; i++)
    {
        ret = pthread_create(&tid[i], NULL, function_pointer[i], NULL); // 스레드 만듦
        if (ret < 0)
        {
            perror("fail to create thread");
            FATAL("is %d", i);
        }

        ret = pthread_detach(tid[i]); // 스레드 분리
        if (ret < 0)
        {
            perror("fail to detach thread");
            FATAL("is %d", i);
        }
    }
}