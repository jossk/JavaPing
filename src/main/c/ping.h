//
// Created by papa on 1/24/21.
//

#ifndef JAVA_PING_PING_H
#define JAVA_PING_PING_H

//#define PING_DEBUG

enum PING_ERROR {
    PING_OK = 0,
    PING_FAILED,
    PING_ERROR_INVALID_HOSTNAME,
    PING_ERROR_GETPROTOBYNAME,
    PING_ERROR_GETHOSTBYNAME,
    PING_ERROR_SOCKET_OPEN,
    PING_ERROR_SOCKET_SEND,
    PING_ERROR_SOCKET_RECV,
    PING_ERROR_INVALID_RECV
};


int ping4(const char *hostname, int timeout_sec);

#endif //JAVA_PING_PING_H