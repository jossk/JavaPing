#include <memory.h>

#include <unistd.h>

#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>

#include "ping.h"

#ifdef PING_DEBUG
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define PACKET_SIZE 4096


static unsigned short calc_chksum(unsigned short *addr, int len) {
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }


    if (nleft == 1) {
        *(unsigned char *) (&answer) = *(unsigned char *) w;
        sum += answer;

    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}


static void tv_sub(struct timeval *out, struct timeval *in) {
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    } out->tv_sec -= in->tv_sec;
}


int ping4(const char *hostname, int timeout_sec) {
    if (! hostname) {
        return PING_ERROR_INVALID_HOSTNAME;
    }

    TRACE("=== get PID\n");
    pid_t pid = getpid();
    TRACE("pid:%d\n", (int) pid);

    TRACE("=== getprotobyname(ICMP)\n");
    struct protoent *proto = getprotobyname("ICMP");
    if (!proto) {
        perror("getprotobyname(ICMP) is FAIL!");
        return PING_ERROR_GETPROTOBYNAME;
    }
    printf("proto:%p\n", proto);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));

    TRACE("=== inet_addr(%s)\n", hostname);
    unsigned long inaddr = inet_addr(hostname);
    TRACE("inaddr=%ld\n", inaddr);
    if (inaddr == INADDR_NONE) {
        TRACE("=== get gethostbyname(%s)\n", hostname);
        struct hostent *hname = gethostbyname(hostname);
        if (!hname) {
            perror("gethostbyname() is FAIL");
            return PING_ERROR_GETHOSTBYNAME;
        }
        addr.sin_family = hname->h_addrtype;
        memcpy((char*) &(addr.sin_addr), hname->h_addr, hname->h_length);
    } else {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inaddr;
    }
    TRACE("sin_port:%d sin_family:%d s_addr:%ld\n", addr.sin_port, addr.sin_family, addr.sin_addr.s_addr);

    int error = PING_OK;
    int socketraw = -1;

    do {
        TRACE("=== open socket\n");
        socketraw = socket(AF_INET, SOCK_RAW, proto->p_proto);
        if (socketraw < 0) {
            perror("socket open error!");
            error = PING_ERROR_SOCKET_OPEN;
            break;
        }

        // Set time for the socket to timeout and give up waiting for a reply.
        struct timeval wait;
        wait.tv_sec  = timeout_sec;
        wait.tv_usec = 0;
        setsockopt(socketraw, SOL_SOCKET, SO_RCVTIMEO, (void *)&wait, sizeof (struct timeval));

        TRACE("== prepare packet\n");
        char sendpacket[PACKET_SIZE];
        int packsize = 0;
        int datalen = 56;
        struct icmp *icmp = (struct icmp *)sendpacket;
        icmp->icmp_type = ICMP_ECHO;
        icmp->icmp_code = 0;
        icmp->icmp_cksum = 0;
        icmp->icmp_seq = 0; // pack_no;
        icmp->icmp_id = pid;
        packsize = 8+datalen;
        struct timeval *tval = (struct timeval*)icmp->icmp_data;
        gettimeofday(tval, NULL);
        icmp->icmp_cksum = calc_chksum((unsigned short*)sendpacket, packsize);

        TRACE("== send packet\n");
        ssize_t send_res = sendto(socketraw, sendpacket, packsize, 0, (struct sockaddr*)&addr, sizeof(addr));
        if (send_res < 0) {
            perror("socket send error!");
            error = PING_ERROR_SOCKET_SEND;
            break;
        }

        TRACE("== recv packet\n");
        char recvpacket[PACKET_SIZE];
        struct sockaddr_in from;
        int fromlen = sizeof(from);
        ssize_t recv_res = recvfrom(socketraw, recvpacket, sizeof(recvpacket), 0, (struct sockaddr*)&from, &fromlen);
        if (recv_res < 0) {
            perror("socket recv error!");
            error = PING_ERROR_SOCKET_RECV;
            break;

        }
        struct timeval tvrecv;
        gettimeofday(&tvrecv, NULL);

        TRACE("== check recv packet\n");
        struct ip *ip = (struct ip*)recvpacket;
        int iphdrlen = ip->ip_hl << 2;
        icmp = (struct icmp*)(&recvpacket[iphdrlen]);
        int len = recv_res - iphdrlen;
        TRACE("recv_res=%d len=%d\n", recv_res, len);
        if (len < 8) {
            TRACE("recv ICMP packet length is less than 8\n");
            error = PING_ERROR_INVALID_RECV;
            break;
        }
        if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid)) {
            struct timeval *tvsend = (struct timeval*)icmp->icmp_data;
            tv_sub(&tvrecv, tvsend);
            double rtt = tvrecv.tv_sec * 1000 + tvrecv.tv_usec / 1000;
            TRACE("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n", len, inet_ntoa(from.sin_addr), icmp->icmp_seq, ip->ip_ttl, rtt);
            error = PING_OK;
        } else {
            TRACE("icmp_type:%d icmp_id:%d\n", icmp->icmp_type, icmp->icmp_id);
            error = PING_ERROR_INVALID_RECV;
        }
    } while(0);


    if (!(socketraw < 0) ) {
        TRACE("close socket %d\n", socketraw);
        close(socketraw);
    }

    TRACE("return %d\n", error);
    return error;
}
