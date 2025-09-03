
#include "pcom.h"
#include <stdio.h>

const char* print_error(int err) {
    static char buf[256];
    pcom_error_text(err, buf, sizeof(buf));
    return buf;
}

int main() {
    int sfd = pcom_server_open("pcomtest");
    if (sfd < 0) {
        printf("Server open failed: %s\n", print_error(sfd));
        return 1;
    }

    printf("Waiting for connection...\n");
    int cfd = pcom_server_accept(sfd);
    if (cfd < 0) {
        printf("Accept failed: %s\n", print_error(cfd));
        return 1;
    }

    pcom_user_info_t user_info;
    if (pcom_server_check_user(cfd, &user_info) < 0) {
        printf("User check failed: %s\n", print_error(cfd));
        return 1;
    }
    printf("User: %s, Groups: %d\n", user_info.username, user_info.group_count);
    for (int i = 0; i < user_info.group_count; ++i) {
        printf("Group %d: %s\n", i, user_info.group_list[i]);
    }
    printf("Is admin: %d\n", user_info.is_admin);
    printf("Waiting for data...\n");

    char buf[128] = {0};
    int r = pcom_server_recv(cfd, buf, sizeof(buf));
    printf("Received: %.*s\n", r, buf);

    pcom_server_send(cfd, "OK", 2);
    pcom_server_close(cfd);
    pcom_server_close(sfd);
    return 0;
}
