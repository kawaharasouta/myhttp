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

char *memstr(char *buf, int blen, char *target, int tlen);

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
	char *addr = "160.16.62.54";

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "socket error");
		exit(1);
	}
	printf("ソケット生成\n");

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	server.sin_addr.s_addr = inet_addr(addr) ;

	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
		fprintf(stderr, "connect error");
		close(socket);
		exit(1);
	}
	printf("connect successed\n");

	char send_buf[1024];
	strcpy(send_buf, "GET /index.html HTTP/1.0\r\n\r\n");
	if(send(sock, send_buf, strlen(send_buf), 0) < 0){
		fprintf(stderr, "send error");
		close(sock);
		exit(1);
	}
	printf("send successed\n");

	//!保存するファイル名
	FILE *fp;
	if((fp = fopen("test.html", "w")) == NULL){
		fprintf(stderr, "fopen error");
		exit(1);
	}
	printf("file open\n");
	
	//!受け取る用バフ
	char tmpbuf[8193], *ptr;
	fd_set read, mask;//ソケット見張るやつ selectが毎回書き換えするので2つ宣言する???
	FD_ZERO(&mask);
	FD_SET(sock, &mask);//maskに通信してるソケットを設定
	//!head:ヘッダー超えたら1になるっぽい end:終了フラグっぽい
	int head, end;
	//!size of receved message
	int size;
	head = 0;
	end = 0;
	//!time
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	//while(1){//レスポンスを受け取るん
		//read = mask;//なぜここで代入作業がはいるんだ
		printf("selectします\n");
		//FD_ZERO(&mask);
		//FD_SET(sock, &mask);//maskに通信してるソケットを設定
		switch(select(sock + 1, &mask, NULL, NULL, &timeout)){
			case -1:
				fprintf(stderr, "select\n");
				end = 1;
				break;
			case 0:
				printf("なんも来てないです\n");
				break;
			default:
				if(FD_ISSET(sock, &mask)){
					printf("メッセージ受信\n");
					recv(sock, tmpbuf, sizeof(tmpbuf), 0);//メッセージの長さが帰ってくる
					size = strlen(tmpbuf);
					if(size <= 0){//0以下の時は正常実行されてない時らしいけど。
						printf("終了\n");
						end = 1;
					}
					else{
						if(head == 0){//ヘッダーだったら
							ptr = memstr(tmpbuf, size, "\r\n\r\n", 4);//ptrにヘッダー以降が入る
							//printf("ptr:%s\n",ptr);
							if(ptr != NULL){
								printf("ヘッダ部かつ中身あり\n");
								//ヘッダ部を標準エラー出力に送るみたい
								//fwrite(tmpbuf, ptr - tmpbuf + 4, 1, stderr);
								//fwrite(tmpbuf, strlen(tmpbuf) - strlen(ptr), 1, fp);
								fwrite(ptr, strlen(ptr), 1, fp);
								//fwrite("\n\n", 4, 1, fp);
								//printf("tmpbuf:%d ptr:%d\n", strlen(tmpbuf), strlen(ptr));
								head = 1;
								//data
								//fwrite(ptr, size, 1, fp);
							}
							else{
								printf("ヘッダ部かつ中身なし\n");
								fwrite(tmpbuf, size, 1, stderr);
							}
							//printf("get hedder\n");
						}
						else{//ヘッダおわってるからデータ
							//printf("head:%d\n",head);
							//printf("size:%d\n", size);
							printf("データのみ\n");
							printf("fwrite:%d\n",(int)fwrite(tmpbuf, strlen(tmpbuf), 1, fp));
							//printf("get data\n");
							//printf("data\n%s", tmpbuf);
						}
					}
				}
				break;
		}
	//	if(end == 1) break;
	//}
	fclose(fp);
	close(sock);
	return 0;
}

/**
 * @fn memstr
 * @brief binary version of strstr()
 * @param buf:in str, target:return, 
 * @return char*
 * @sa main
 */
char *memstr(char *buf, int blen, char *target, int tlen){
	int i,j;
	//!flug
	int ok; 

	for(i = 0;i < blen;i++){
		if(buf[i] == target[0]){
			ok = 1;
			for(j = 1;j < tlen;j++){
				if(buf[i + j] != target[j]){
					  ok = 0;
					  break;
				}   
			}
			if(ok == 1){
				return(buf + i);
			}
		}   
	}   
	return(NULL);
}
