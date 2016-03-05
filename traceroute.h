#ifndef TRACE_H
#define TRACE_H

#include <arpa/inet.h>

#define MAX_TTL 30
#define TIMEOUT 1000

int trace(sockaddr_in &recipient, u_int16_t pid);

#endif // TRACE_H
