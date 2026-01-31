
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#include "string.h"



//#define DEBUG



static struct sockaddr_in sockaddr_recv;
static struct sockaddr_in sockaddr_send;
static int socket_fd = -1;

static bool socket_read_ready_inms(int timeout_ms) {
    struct pollfd pfd;
    pfd.fd = socket_fd;
    pfd.events = POLLIN;
    nfds_t num_fds = 1;

    return poll(&pfd, num_fds, timeout_ms) == 1;
}


static ssize_t send_packet(char* bytes, size_t size) {
    return sendto(socket_fd,
            bytes,
            size,
            0, // flags
            (struct sockaddr*)&sockaddr_send, 
            sizeof(sockaddr_send));
}

static ssize_t recv_packet(char* outbuf, size_t outbuf_max) {
 
    if(!socket_read_ready_inms(6*1000)) {
        return -1; // Timed out.
    }

    socklen_t slen = sizeof(sockaddr_recv);
    return recvfrom(
            socket_fd,
            outbuf,
            outbuf_max,
            0, // flags
            (struct sockaddr*)&sockaddr_recv,
            &slen);
}

static void dump_hex(const char* label, char* buffer, size_t size) {
    printf("\033[32m(%s)\033[90m: \033[0m", label);
    for(size_t i = 0; i < size; i++) {
        printf("%02X ", (uint8_t)buffer[i]);
    }
    printf("\n");
}


bool get_server_a2s_responses(const char* host, uint16_t query_port,
        struct string_t* server_info,
        struct string_t* server_mod_info
){
    bool result = false;
   
    // Response packet is used store the previous sent packet and challenge number
    size_t response_size = 0;
    char* response_packet = NULL;


    //printf("%s: %s:%i\n", __func__, host, query_port);

    socket_fd = -1;
    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_fd < 0) {
        fprintf(stderr, "Failed to open socket | %s\n", strerror(errno));
        goto out;
    }

    explicit_bzero(&sockaddr_recv, sizeof(sockaddr_recv));
    explicit_bzero(&sockaddr_send, sizeof(sockaddr_send));

    sockaddr_send.sin_family = AF_INET;
    sockaddr_send.sin_port = htons(query_port);
    inet_pton(AF_INET, host, &sockaddr_send.sin_addr);


    sockaddr_recv.sin_family = AF_INET;
    sockaddr_recv.sin_port = htons(query_port);
    sockaddr_recv.sin_addr.s_addr = htonl(INADDR_ANY);

    #define CHALLENGE_NUM_DATA_SIZE 9

    ssize_t recv_numbytes = 0;
    char    challenge_resp [CHALLENGE_NUM_DATA_SIZE] = { 0 };

    // -5 because dont include first 5 bytes (FF FF FF FF 41)
    const int challenge_num_len = CHALLENGE_NUM_DATA_SIZE - 5; 

    // https://developer.valvesoftware.com/wiki/Server_queries

    char info_packet[] = {
        0xff, 0xff, 0xff, 0xff, 0x54, 0x53, 0x6f, 0x75, 0x72, 0x63,
        0x65, 0x20, 0x45, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x20, 0x51, 0x75, 0x65, 0x72, 0x79, 0x0
    };

#ifdef DEBUG
    dump_hex("general info packet", info_packet, sizeof(info_packet));
#endif

    if(send_packet(info_packet, sizeof(info_packet)) < 0) {
        fprintf(stderr, "Failed to send general info packet | %s\n", strerror(errno));
        goto out;
    }


    recv_numbytes = recv_packet(challenge_resp, sizeof(challenge_resp));
    if(recv_numbytes < CHALLENGE_NUM_DATA_SIZE) {
        fprintf(stderr, "Failed to receive challenge "
                "token/number for general info | %s\n", strerror(errno));
        goto out;
    }

#ifdef DEBUG
    dump_hex("general info token", challenge_resp, recv_numbytes);
#endif

    // Ok so it should have received the challenge number/token
    // now we need to append the challenge number at end of the info packet.

    response_size = sizeof(info_packet) + challenge_num_len;
    response_packet = calloc(1, response_size);

    memmove(response_packet, info_packet, sizeof(info_packet));
    memmove(response_packet + sizeof(info_packet),
            &challenge_resp[5], // Ignore first 5 bytes.
            challenge_num_len);

#ifdef DEBUG
    dump_hex("general info resp", response_packet, response_size);
#endif

    if(send_packet(response_packet, response_size) < 0) {
        fprintf(stderr, "Failed to send info packet "
                "with challenge number | %s\n", strerror(errno));
        goto out;
    }

    free(response_packet);
    response_packet = NULL;
    memset(challenge_resp, 0, sizeof(challenge_resp));

    // At this point we should be able to get received general info about the server.
    string_reserve(server_info, 1024);
    server_info->size = recv_packet(server_info->bytes, server_info->mem_size);




    // Now try to get mod info of the server.

    char modinfo_packet[] = { 
        0xff, 0xff, 0xff, 0xff, 0x56, 0x0, 0x0, 0x0, 0x0
    };

#ifdef DEBUG
    dump_hex("mod info packet", modinfo_packet, sizeof(modinfo_packet));
#endif

    if(send_packet(modinfo_packet, sizeof(modinfo_packet)) < 0) {
        fprintf(stderr, "Failed to send mod info packet | %s\n", strerror(errno));
    }

    recv_numbytes = recv_packet(challenge_resp, sizeof(challenge_resp));
    if(recv_numbytes < CHALLENGE_NUM_DATA_SIZE) {
        fprintf(stderr, "Failed to receive challenge "
                "token/number for mod info | %s\n", strerror(errno));
        goto out;
    }

#ifdef DEBUG
    dump_hex("mod info token", challenge_resp, sizeof(challenge_resp));
#endif
    // The challenge number/token should be now received.
    // It is the same thing again as previous step, 
    // append the received token back to the just sent modinfo_packet and send it.

    response_size = sizeof(modinfo_packet) + challenge_num_len;
    response_packet = calloc(1, response_size);

    memmove(response_packet, modinfo_packet, sizeof(modinfo_packet) - 4);
    memmove(response_packet + sizeof(modinfo_packet) - 4,
            &challenge_resp[5],
            challenge_num_len);

#ifdef DEBUG
    dump_hex("mod info resp", response_packet, sizeof(modinfo_packet));
#endif

    if(send_packet(response_packet, sizeof(modinfo_packet)) < 0) {
        fprintf(stderr, "Failed to send mod info packet "
                "with challenge number | %s\n", strerror(errno));
        goto out;
    }


    string_reserve(server_mod_info, 1024);
    server_mod_info->size = recv_packet(server_mod_info->bytes, server_mod_info->mem_size);


    // Need to implement this to be able to merge multiple chunks.
    // But for now it will just print warning that its not implemented yet.
    const uint8_t total_chunks = server_mod_info->bytes[7];

    if(total_chunks != 1) {
        fprintf(stderr,
                "\033[31m\n"
                "=================================================================\n"
                " [ WARNING ]: There seems to be more than 1 chunks in the packet.\n"
                "              This is not an error,\n"
                "              but chunk merging is not implemented yet.\n"
                "              Check if there is any updates.\n"
                "=================================================================\n"
                "\033[0m\n"
                );
        goto out;
    }


    result = true;

out:

    if(response_packet) {
        free(response_packet);
        response_packet = NULL;
    }

    if(socket_fd >= 0) {
        close(socket_fd);
    }

    return result;
}


