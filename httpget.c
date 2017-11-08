/**
 * @file httpget.c
 * @brief myhttp_client
 * @author khwarizmi
 */

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

/**
 * @fn main
 * @brief main
 * @param for main
 * @return int status
 * @sa any func
 */
int main(int argc, char *argv[]){
	//!ソケット ファイルディスクリプタ
	int sock;
	char *addr = "127.0.0.1";

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "socket error");
		exit(1);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	server.sin_addr.s_addr = inet_addr(addr) ;

	if(connect(socket, (struct sockaddr *)&server, sizeof(server)) < 0){
		fprintf(stderr, "connect error");
		close(socket);
		exit(1);
	}
}
