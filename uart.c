#include "uart.h"

uart_t ut[UART_MAX_NUM] = {
    0,
};

static int init_uart_structure(UART_MEMBER member)
{
    int ret = 0;
    struct termios term = {0, };
    char *uart_route;

    switch (member)
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

    ret = sem_init(&ut[member].sem, 0, 1);
    if(ret < 0)
    {
        perror("fail to init semaphore");
        return -1;
    }

    ret = open(uart_route, O_RDWR | O_NOCTTY);
    if(ret < 0)
    {
        perror("fail to open uart");
        return -1;
    }

    ut[member].uart_fd = ret;

    term.c_cc[VTIME] = 0;
    term.c_cc[VMIN] = 0;
    term.c_cflag = B115200 | CREAD | CS8 | CLOCAL & ~CSTOPB;
    term.c_iflag = IGNPAR;
    
    ret = tcflush(ut[member].uart_fd, TCIOFLUSH);
    if(ret < 0)
    {
        perror("fail to flush");
        return -1;
    }

    ret = tcsetattr(ut[member].uart_fd, TCSANOW, &term);
    if(ret < 0)
    {
        perror("fail to set attr");
        return -1;
    }

    return 1;

}

static int init_uart()
{
    int ret = 0;
    
    ret = init_uart_structure(UART1);
    if(ret < 0)
    {
        return -1;
    }

    return 1;
}

static int uart_recv(UART_MEMBER mem, void *recv, size_t recv_size)
{
    int ret = 0;
    
    ret = read(ut[mem].uart_fd, recv, recv_size);
    if(ret < 0)
    {
        perror("fail to read data");
        return ret;
    }

    return ret;
}

static int uart_send(UART_MEMBER mem, void *send, size_t send_size)
{
    int ret = 0;

    ret = write(ut[mem].uart_fd, send, send_size);
    if(ret < 0)
    {
        perror("fail to write data");
        return ret;
    }

    return ret;
}

static int thread_uart1_rx()
{
    char uart_rx[UART_TEMP_BUFFER_SIZE] = {0, };
    int rx_size = 0;

    while(1)
    {
        memset(uart_rx, 0, sizeof(uart_rx));
        rx_size = uart_recv(UART1, uart_rx, sizeof(uart_rx));
        if(rx_size < 0)
        {
            FATAL("fail to recv");
        }

        else if(rx_size > 0)
        {
            DEBUG("recv : %s", uart_rx);
        }
    }
}

static int thread_uart1_tx()
{
    char uart_tx[UART_TEMP_BUFFER_SIZE] = {0, };
    int tx_size = 0;
    int ret = 0;

    while(1)
    {
        memset(uart_tx, 0, sizeof(uart_tx));

        ret = scanf("%s", uart_tx);
        if(ret != 1)
        {
            FATAL("fail to scanf");
        }

        tx_size = uart_send(UART1, uart_tx, strlen(uart_tx));
        if(tx_size < 0)
        {
            FATAL("fail to recv");
        }
    }
}

void start_uart_thread()
{
    pthread_t tid[UART_THREAD_NUM] = {0, };
    void (*function_pointer[UART_THREAD_NUM]) = {&thread_uart1_rx, &thread_uart1_tx};
    uint8_t i = 0;
    int ret = 0;

    init_uart();

    for(i = 0; i < UART_THREAD_NUM; i++)
    {
        ret = pthread_create(&tid[i], NULL, function_pointer[i], NULL);
        if(ret < 0)
        {
            perror("fail to create thread");
            FATAL("is %d", i);
        }

        ret = pthread_detach(tid[i]);
        if(ret < 0)
        {
            perror("fail to detach thread");
            FATAL("is %d", i);
        }
    }
}