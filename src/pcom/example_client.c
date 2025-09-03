
#include "pcom.h"
#include <stdio.h>
#include <string.h>

const char* print_error(int err) {
    static char buf[256];
    pcom_error_text(err, buf, sizeof(buf));
    return buf;
}

int main() {
    int fd = pcom_client_open("pcomtest");
    if (fd < 0) {
        printf("Client open failed: %s\n", print_error(fd));
        return 1;
    }

    pcom_client_send(fd, "Hello", 5);

    char buf[128] = {0};
    int r = pcom_client_recv(fd, buf, sizeof(buf));
    printf("Reply: \'%.*s\'\n", r, buf);

    pcom_client_close(fd);
    return 0;
}
