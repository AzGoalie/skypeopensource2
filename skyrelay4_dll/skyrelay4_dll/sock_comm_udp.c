//
// Socket communication
//

#include <stdio.h>
#include <stdlib.h>

#include <winsock.h>  

int tcpsock;
int tcpconnected=0;


int bind_local=0;

#define FROM_PORT 33999
//#define FROM_PORT 14410


//
// unified udp communication, send buffer, and place retrived data into result.

int udp_talk(char *remoteip, unsigned short remoteport, char *buf, int len, char *result, int result_maxlen, int *retsock){
    int s, ret, addrlen;
    struct sockaddr_in addr;
    WSADATA wsaDATA;
    fd_set rfds;
    struct timeval tv;
    struct sockaddr_in src_addr;

    tv.tv_sec=15;
    tv.tv_usec=0;

    if (WSAStartup(MAKEWORD(2,0),&wsaDATA)!=0){
        //printf("WSAStartup error. Error: %d\n",WSAGetLastError());
        return -1;
    };

    s=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (s==-1) {
        printf("Socket creation error\n");
        return -1;
    };

    
    if (bind_local) {
        // bind local port
        memset(&src_addr,0,sizeof(src_addr));
        src_addr.sin_family=AF_INET;
        src_addr.sin_port=htons(FROM_PORT);
        src_addr.sin_addr.s_addr=INADDR_ANY;
        ret=bind(s,(struct sockaddr *)&src_addr,sizeof(struct sockaddr_in));
        if (ret==-1) {
            printf("Bind localport failed\n");
            printf("WSAStartup error. Error: %d\n",WSAGetLastError());
            return -1;
        };
    };

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr=inet_addr(remoteip);
    addr.sin_port=htons(remoteport);

    ret=sendto(s, buf, len, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        printf("Sendto error\n");
        shutdown(s,2);
        closesocket(s);
        *retsock=s;
        return -1;
    };
    if (ret != len) {
        printf("udp send error\n");
        return -1;
    };

    addrlen=sizeof(addr);

    memset(result,0,result_maxlen);

    FD_ZERO(&rfds);
    FD_SET(s, &rfds);

    select(s+1, &rfds, NULL, NULL, &tv);

    if(FD_ISSET(s, &rfds)){
        ret=recvfrom(s, result, result_maxlen, 0, (struct sockaddr*)&addr, &addrlen);
        if (ret<0){         
            //printf("Recvfrom error\n");
            //printf("Error: recvfrom, Error code %d\n",WSAGetLastError());
            //10054 - Connection reset by peer
            //if (WSAGetLastError()==10054) return -2;
            //return -1;
            //shutdown(s,2);
            //closesocket(s);
            *retsock=s;
            return -1;
        };

    }else{ 
        //shutdown(s,2);
        //closesocket(s);
        //timeout
        *retsock=s;
        return 0;
    };
    
    


    //shutdown(s,2);
    //closesocket(s);
    *retsock=s;
    

    return ret;
};


// unified udp communication, place retrived data into result.
int udp_recv(char *remoteip, unsigned short remoteport, char *result, int result_maxlen, int *retsock){
    int s, ret, addrlen;
    struct sockaddr_in addr;
    //WSADATA wsaDATA;
    fd_set rfds;
    struct timeval tv;
    //struct sockaddr_in src_addr;

    tv.tv_sec=5;
    tv.tv_usec=0;


    //if (WSAStartup(MAKEWORD(2,0),&wsaDATA)!=0){
        //printf("WSAStartup error. Error: %d\n",WSAGetLastError());
        //return -1;
    //};

    s=*retsock;

    /*
    s=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (s==-1) {
        //printf("Socket creation error\n");
        return -1;
    };

    if (bind_local) {
        // bind local port
        memset(&src_addr,0,sizeof(src_addr));
        src_addr.sin_family=AF_INET;
        src_addr.sin_port=htons(FROM_PORT);
        src_addr.sin_addr.s_addr=INADDR_ANY;
        ret=bind(s,(struct sockaddr *)&src_addr,sizeof(struct sockaddr_in));
        if (ret==-1) {
            //printf("Bind localport failed\n");
            return -1;
        };
    };
    */

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr=inet_addr(remoteip);
    addr.sin_port=htons(remoteport);

    addrlen=sizeof(addr);

    memset(result,0,result_maxlen);

    FD_ZERO(&rfds);
    FD_SET(s, &rfds);

    select(s+1, &rfds, NULL, NULL, &tv);

    if(FD_ISSET(s, &rfds)){
        ret=recvfrom(s, result, result_maxlen, 0, (struct sockaddr*)&addr, &addrlen);
        if (ret<0){         
            //printf("Recvfrom error\n");
            //printf("Error: recvfrom, Error code %d\n",WSAGetLastError());
            //10054 - Connection reset by peer
            //if (WSAGetLastError()==10054) return -2;
            //return -1;
            //shutdown(s,2);
            //closesocket(s);
            *retsock=s;
            return -1;
        };

    }else{ 
        //shutdown(s,2);
        //closesocket(s);
        //timeout
        *retsock=s;
        return 0;
    };
    

    //shutdown(s,2);
    //closesocket(s);
    *retsock=s;


    return ret;
};



//
// unified udp communication, send buffer, and place retrived data into result.
int udp_talk_more(char *remoteip, unsigned short remoteport, char *buf, int len, char *result, int result_maxlen){
    int s, ret, addrlen;
    struct sockaddr_in addr;
    WSADATA wsaDATA;
    char tmpbuf[0x1000];
    fd_set rfds;
    struct timeval tv;
//  struct sockaddr_in src_addr;
    int ret2;

    tv.tv_sec=5;
    tv.tv_usec=0;

    if (WSAStartup(MAKEWORD(2,0),&wsaDATA)!=0){
        //printf("WSAStartup error. Error: %d\n",WSAGetLastError());
        return -1;
    };

    s=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (s==-1) {
        //printf("Socket creation error\n");
        return -1;
    };

    /*
    if (bind_local) {
        // bind local port
        memset(&src_addr,0,sizeof(src_addr));
        src_addr.sin_family=AF_INET;
        src_addr.sin_port=htons(FROM_PORT);
        src_addr.sin_addr.s_addr=INADDR_ANY;
        ret=bind(s,(struct sockaddr *)&src_addr,sizeof(struct sockaddr_in));
        if (ret==-1) {
            //printf("Bind localport failed\n");
            return -1;
        };
    };
    */

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr=inet_addr(remoteip);
    addr.sin_port=htons(remoteport);

    ret=sendto(s, buf, len, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        //printf("Sendto error\n");
        shutdown(s,2);
        closesocket(s);
        return -1;
    };

    addrlen=sizeof(addr);

    //printf("udp\n");

    ret=0;
    do {
     ret2=0;

     memset(tmpbuf,0,sizeof(tmpbuf)-1);
     FD_ZERO(&rfds);
     FD_SET(s, &rfds);
     select(s+1, &rfds, NULL, NULL, &tv);
     if(FD_ISSET(s, &rfds)){
        ret2=recvfrom(s, tmpbuf, sizeof(tmpbuf)-1, 0, (struct sockaddr*)&addr, &addrlen);
        //printf("ret2: 0x%08X, ret: 0x%08X\n",ret2,ret);
        if (ret2<0){            
            //printf("Recvfrom error\n");
            //printf("Error: recvfrom, Error code %d\n",WSAGetLastError());
            //10054 - Connection reset by peer
            //if (WSAGetLastError()==10054) return -2;
            //return -1;
            shutdown(s,2);
            closesocket(s);
            return ret;
        };

        // for not overrun buffer
        // kast pkt not write
        if (ret+ret2>result_maxlen){
            shutdown(s,2);
            closesocket(s);
            return ret;
        };

        memcpy(result+ret, tmpbuf, ret2);
        ret=ret+ret2;

     }else{ 
        shutdown(s,2);
        closesocket(s);
        //no more data 
        return ret;
     };
    
    //end of while
    }while (ret2>0);



    shutdown(s,2);
    closesocket(s);
    

    return ret;
};


