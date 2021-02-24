#ifndef MU_SOCKUTILS_H
#define MU_SOCKUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

int mu_sock_writen(int fd, const void *vptr, int n, double timeout);

int mu_sock_readn(int fd, void *vptr, int n, double timeout);

int mu_sock_write_string(int sock, char *s);

int mu_sock_printf(int sock, char *fmt, ...);

int mu_sock_connect(char *host, int port);

long int mu_sock_bytes_available(int sock);

int mu_sock_clear_input_buffer(int sock);

#ifdef __cplusplus
}
#endif

#endif
