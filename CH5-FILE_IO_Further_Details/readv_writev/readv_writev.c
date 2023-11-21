#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct my_iov {
    void *buf;
    size_t length;
} my_iov;

ssize_t my_readv(int fileDes, const my_iov *iov, size_t iovcnt) {
    if (iovcnt < 1) {
        errno = EINVAL;
        perror("iovcnt");
        return -1;
    }

    ssize_t totalBytesRead = 0;

    for (int i = 0; i < iovcnt; i++) {
        ssize_t bytesRead = read(fileDes, iov[i].buf, iov[i].length);

        if (bytesRead == -1) {
            perror("read");
            return -1;
        }

        totalBytesRead += bytesRead;

        // Check if the total length exceeds SSIZE_MAX
        if (totalBytesRead >= SSIZE_MAX) {
            errno = EINVAL;
            perror("total length exceeds SSIZE_MAX");
            return -1;
        }
    }

    return totalBytesRead;
}

ssize_t my_writev(int fileDes, const my_iov *iov, size_t iovcnt) {
    if (iovcnt < 1) {
        errno = EINVAL;
        perror("iovcnt");
        return -1;
    }

    ssize_t totalBytesWritten = 0;

    for (int i = 0; i < iovcnt; i++) {
        ssize_t bytesWritten = write(fileDes, iov[i].buf, iov[i].length);

        if (bytesWritten == -1) {
            perror("write");
            return -1;
        }

        totalBytesWritten += bytesWritten;

        // Check if the total length exceeds SSIZE_MAX
        if (totalBytesWritten >= SSIZE_MAX) {
            errno = EINVAL;
            perror("total length exceeds SSIZE_MAX");
            return -1;
        }
    }

    return totalBytesWritten;
}
