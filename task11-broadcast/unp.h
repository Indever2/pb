//lib/unp.h
 /* Наш собственный заголовочный файл */
//#ifndef __unp_h
#define __unp_h
#include <net/if.h>
#include <sys/mman.h>
#include <sys/types.h> /* основные системные типы данных */
#include <sys/socket.h> /* основные определения сокетов */
#include <sys/time.h> /* структура timeval{} для функции select() */
#include <time.h> /* структура timespec{} для функции pselect() */
#include <netinet/in.h> /* структура sockaddr_in{} и другие сетевые
                               определения */
#include <netpacket/packet.h>
//#include <pcap.h> /* для захвата пакетов */
#include <netinet/if_ether.h> 
#include <arpa/inet.h> /* inet(3) функции */
#include <errno.h>
#include <fcntl.h> /* для неблокируемых сокетов */
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* для констант S_xxx */
#include <sys/uio.h> /* для структуры iovec{} и ready/writev */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h> /* для доменных сокетов Unix */
#include <fcntl.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h> /* для удобства */
#endif
#ifdef HAVE_SYS_SYSCTL_H
#include <sys/sysctl.h>
#endif
#ifdef HAVE_POLL_H
#include <poll.h> /* для удобства */
#endif
#ifdef HAVE_SYS_EVENT_H
#include <sys/event.h> /* для kqueue */
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h> /* для удобства */
#endif
/* Три заголовочных файла обычно нужны для вызова ioctl
 42    для сокета/файла: <sys/ioctl.h>, <sys/filio.h>,
    <sys/sockio.h> */
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#ifdef HAVE_NET_IF_DL_H
#include <net/if_dl.h>
#endif
#ifdef HAVE_NETINET_SCTP_H
#include <netinet/sctp.h>
#endif
/* OSF/1 фактически запрещает recv() и send() в <sys/socket.h> */
#ifdef __osf__
#undef recv
#undef send
#define recv(a,b,c,d) recvfrom(a,b,c,d,0,0)
#define send(a,b,c,d) sendto(a,b,c,d,0,0)
#endif
/* Структура, возвращаемая функцией recvfrom_flags() */
/* Нам нужны более новые макросы CMSG_LEN() и CMSG_SPACE(), но в 
настоящее время их поддерживают далеко не все реализации. Им требуется
макрос ALIGN(), но это зависит от реализации */
#ifndef CMSG_LEN
#define CMSG_LEN(size) (sizeof(struct cmsghdr) + (size))
#endif
#ifndef CMSG_SPACE
#define CMSG_SPACE(size) (sizeof(struct cmsghdr) + (size))
#endif
/* POSIX требует макрос SUN_LEN(), но он определен
не во всех реализациях. Этот макрос 4.4BSD работает
независимо от того, имеется ли поле длины */
#ifndef SUN_LEN
#define SUN_LEN(su) 
(sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
#endif
/* Это значение можно было бы извлечь из SOMAXCONN в <sys/socket.h>,
   но многие ядра по-прежнему определяют его как 5,
   хотя на самом деле поддерживается гораздо больше */
#define LISTENQ 3 /* второй аргумент функции listen() */
/* Различные константы */
#define MAXLINE  4096 /* максимальная длина текстовой строки */
#define BUFFSIZE 8192 /* размер буфера для чтения и записи */
/* Определение номера порта, который может быть использован для
   взаимодействия клиент-сервер */
#define SERV_PORT      9877  /* клиенты и серверы TCP и UDP */
#define SERV_PORT_STR "9877" /* клиенты и серверы TCP и UDP */
#define UNIXSTR_PATH "/tmp/unix.str" /* потоковые клиенты и серверы
                                        домена Unix */
#define UNIXDG_PATH "/tmp/unix.dg" /* клиенты и серверы протокола
                                      дейтаграмм домена Unix */
/* Дальнейшие определения сокращают преобразования типов
   аргументов-указателей */
#define SA struct sockaddr
#define HAVE_STRUCT_SOCKADDR_STORAGE
#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
/*
 * RFC 3493: протокольно-независимая структура адреса сокета
 */
#define __SS_MAXSIZE 128
#define __SS_ALIGNSIZE (sizeof(int64_t))
#ifndef HAVE_SOCKADDR_SA_LEN
#define __SS_PADS1SIZE (__SS_ALIGNSIZE - sizeof(u_char) - sizeof(sa_family_t))
#else
#define _SS_PAD1SIZE (__SS_ALIGNSIZE - sizeof(sa_family_t))
#endif
#define __SS_PAD2SIZE (__SS_MAXSIZE — 2*__SS_ALIGNSIZE)
struct sockaddr_storage {
#ifdef HAVE_SOCKADDR_SA_LEN
 u_char ss_len;
#endif
sa_family_t ss_family;
char        __ss_pad1[__SS_PAD1SIZE];
int64_t     ss_align;
char        __ss_pad2[_SS_PAD2SIZE];
};
#endif
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
/* заданные по умолчанию разрешения на доступ для новых файлов */
#define DIR_MODE (FILE_MODE | S_IXUSR | S_IXGRP | S_IXOTH)
/* разрешения по умолчанию на доступ к файлам для новых каталогов */
typedef void Sigfunc(int); /* для обработчиков сигналов */
#define min(a, b) ((а) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

//константы, обернутые в функции

int BUF_SIZE() {

    return 64;

}

int PACKET_LEN() {

    return 1024;

}

int SERVER_PORT() {

    return 3333;

}

int CLIENT_PORT() {

    return 3334;

}

int QU_SIZE() {

    return 25;

}

//Операторы, логика, заменители

#define FALSE 0
#define TRUE  1

// (!) Функции - обертки

int Socket(int socket_family, int socket_type, int protocol){

    int sock;

    if ((sock = socket(socket_family, socket_type, protocol)) < 0) {

        perror("socket:\t");
        exit(-1);

    }

    return sock;

} 

void Bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen) {

    if (bind(sockfd, my_addr, addrlen) < 0) {

        perror("bind:\t");
        exit(-1);

    }

} 

void Listen (int s, int backlog) {

    if (listen(s, backlog) < 0) {

        perror("listen:\t");
        exit(3);

    }

}

void Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {

    if (select(nfds, readfds, writefds, exceptfds, timeout) <= 0) {

            perror("select:\t");
            exit(3);

        }

}

void Setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen) {

    if (setsockopt(s, level, optname, optval, optlen) < 0)
        perror("setsockopt(SO_REUSEADDR) failed:\t");

}

void Connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen) {

    if (connect(sockfd, serv_addr, addrlen) < 0) {

        perror("connect:\t");
        exit(-1);

    }

}

ssize_t Sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen) {

    if (sendto(s, msg, len, flags, to, tolen) < 0) {

        perror("sento error:\t");

    } 

}

int Recvfrom(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen) {

    int n = 0;

    if ((n = recvfrom(s, buf, len, flags, from, fromlen)) < 0) {

        perror("recvfrom error:\t");

    } else {

        return n;

    }

}  


/* Обертки libpcap */  /*   

char *Pcap_lookupdev(char *errbuf) {

    char *dev = pcap_lookupdev(errbuf); 
  
    if (dev == NULL) {

        printf("%s\n", errbuf);
        exit(-1);

    } else {

        return dev;

    }   

}

void Pcap_lookupnet(const char *device, bpf_u_int32 *netp, bpf_u_int32 *maskp, char *errbuf) {

    if (pcap_lookupnet(device, netp, maskp, errbuf) < 0) {

        printf("%s\n", errbuf);
        exit(-1);

    }

}

pcap_t *Pcap_open_live (char *device, int snaplen, int promisc, int to_ms, char *errbuf) {

    pcap_t *descr = pcap_open_live(device, snaplen, promisc, to_ms, errbuf);

    if (descr == NULL) {

        printf("pcap_open_live(): %s\n", errbuf);
        exit(-1);

    } else {

        return descr;

    }

}

void Pcap_compile(pcap_t *descr, struct bpf_program *fp, const char *str, int optimize, bpf_u_int32 netmask) {

    if (pcap_compile(descr, fp, str, optimize, netmask) == -1) {

        printf("Error of call pcap_compile function\n");
        exit(-1);

    }

}

void Pcap_setfilter(pcap_t *descr, struct bpf_program *fp) {

    if (pcap_setfilter(descr, fp) == -1) {

        printf("Setting filter error\n");
        exit(-1);

    }  

}

void Pcap_loop(pcap_t *descr, int cnt, pcap_handler callback, u_char *user) {

    if (pcap_loop(descr, cnt, callback, user) < 0) {

        printf("Looping capture error\n");
        exit(-1);

    } 

}
*/

