#define _DEFAULT_SOURCE
#include <errno.h>
#include <stdlib.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#define MQ_TEMPLATE       "/mq-client_%d"
#define MQ_MAX_NAME_LEN   32
#define MQ_CAP            8
#define MQ_FLAGS          (O_CREAT | O_NONBLOCK | O_RDWR)
#define MQ_PERMS          (mode_t)(S_IRUSR | S_IWUSR)

typedef struct {
    mqd_t mq; // Message queue descriptor
    size_t msg_len; // Maximum message length
} ReceiverArgs;

// Receiver thread function
void *receiver_thread(void *arg) {
    ReceiverArgs *receiverArgs = (ReceiverArgs *)arg;
    char *buffer = malloc(receiverArgs->msg_len);
    if (buffer == NULL) {
        perror("malloc");
        pthread_exit(NULL);
    }

    printf("Receiver thread started...\n");
    for (;;) {
        ssize_t received = mq_receive(receiverArgs->mq, buffer, receiverArgs->msg_len, NULL);
        if (received == -1) {
            if (errno == EAGAIN) {
                // No message available, non-blocking mode
                usleep(100000); // Sleep briefly to prevent busy-waiting
                continue;
            } else {
                perror("mq_receive");
                break;
            }
        }

        // Print the received message
        printf("\n[Received]: %s", buffer);
        fflush(stdout);
    }

    free(buffer);
    pthread_exit(NULL);
}

int main() {
    mqd_t ourTerminalMQd, theirTerminalMQd;
    pid_t ourTerminalPID, theirTerminalPID;
    struct mq_attr ourMQAttr;
    char *outputBuffer;
    size_t mqMsgLen = 512;

    outputBuffer = malloc(mqMsgLen);
    if (outputBuffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    ourTerminalPID = getpid();

    char ourMQName[MQ_MAX_NAME_LEN];
    sprintf(ourMQName, MQ_TEMPLATE, ourTerminalPID);

    // Create this terminal's message queue
    ourMQAttr.mq_maxmsg = MQ_CAP;
    ourMQAttr.mq_msgsize = mqMsgLen;

    ourTerminalMQd = mq_open(ourMQName, MQ_FLAGS, MQ_PERMS, &ourMQAttr);
    if (ourTerminalMQd == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("Your Terminal ID: %d\n", ourTerminalPID);
    printf("Waiting to connect to another terminal...\n");

    // Create a receiver thread
    pthread_t receiverThread;
    ReceiverArgs receiverArgs = {ourTerminalMQd, mqMsgLen};
    if (pthread_create(&receiverThread, NULL, receiver_thread, &receiverArgs) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Prompt user to input the other terminal's PID
    printf("Enter the PID of the terminal to connect to: ");
    if (scanf("%d", &theirTerminalPID) != 1 || theirTerminalPID <= 0) {
        fprintf(stderr, "Invalid PID entered.\n");
        exit(EXIT_FAILURE);
    }

    char theirMQName[MQ_MAX_NAME_LEN];
    sprintf(theirMQName, MQ_TEMPLATE, theirTerminalPID);

    // Open their message queue for writing
    theirTerminalMQd = mq_open(theirMQName, O_WRONLY);
    if (theirTerminalMQd == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("Connected to terminal with ID: %d\n", theirTerminalPID);

    // Main loop for sending messages
    for (;;) {
        memset(outputBuffer, '\0', mqMsgLen);

        printf("You: ");
        fflush(stdout);
        if (getline(&outputBuffer, &mqMsgLen, stdin) == -1) {
            perror("getline");
            break;
        }

        if (mq_send(theirTerminalMQd, outputBuffer, mqMsgLen, 0) == -1) {
            perror("mq_send");
            break;
        }
    }

    // Cleanup
    pthread_cancel(receiverThread);
    pthread_join(receiverThread, NULL);

    mq_close(ourTerminalMQd);
    mq_unlink(ourMQName); // Cleanup our message queue
    mq_close(theirTerminalMQd);
    free(outputBuffer);

    exit(EXIT_SUCCESS);
}
