#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <wiringPi.h>

#define TCP_PORT 8088	// 서버 포트 번호 
#define BUFSIZE 1024	// 메시지 버퍼 크기 
#define MAX_CLIENT 3   // 동시 클라이언트 수 

#define LEDPIN 1

struct client_thread_info {
	pthread_t thread;
	struct sockaddr_in client_addr;	
	int sockfd;
	int is_created;
};


/* 스레드 종료시 구조체 미사용 설정을 위한 콜백 함수 */
void client_thread_cleanup(void *aux)
{
	struct client_thread_info *cti = (struct client_thread_info*) aux;

	/* 스레드 구조체 사용 가능 */
	cti->is_created = 0;
}


/* 클라이언트 스레드 함수 */
void *client_thread_loop(void *aux)
{
	struct client_thread_info *cti = (struct client_thread_info*) aux;
	char fromstr[64];
	char buf[BUFSIZE];

	/* 스레드 종료 핸들러 설정 */
	pthread_cleanup_push(client_thread_cleanup, (void*) aux);
	
	/* 네트워크 주소를 문자열로 변경 */
	inet_ntop(AF_INET, &cti->client_addr.sin_addr, fromstr, 64);
	printf("클라이언트 %s 와 연결되었습니다.\n", fromstr);

        /* 클라이언트 loop 시작 */
        do {
                int n;

                /* 클라이언트가 보낸 데이터를 buf로부터 읽음 */
		memset(buf, 0x00, BUFSIZE);
                if ((n = read(cti->sockfd, buf, BUFSIZE)) <= 0) {
                        perror("client thread read()");
                        break;
                }

		/* 클라이언트가 보낸 데이터에 따라 LED 제어 */ 
                printf("클라이언트 %s 에서 보낸 데이터: %s", fromstr, buf);
		if (strncmp("on", buf, 2) == 0) {
			printf("LED를 켭니다\n");
			digitalWrite(LEDPIN, HIGH);
		} else if (strncmp("off", buf, 3) == 0) {
			printf("LED를 끕니다\n");
			digitalWrite(LEDPIN, LOW);
		}

                /* buf에 있는 문자열 전송 */
                if (write(cti->sockfd, buf, n) <= 0) {
                        perror("client thread write()");
                        break;
                }
        } while (strncmp(buf, "q", 1) != 0);

	/* 클라이언트 종료 후 소켓을 닫음 */
	printf("클라이언트 %s와 연결을 종료합니다\n", fromstr);
	close(cti->sockfd);

	pthread_exit((void *) 0);
	pthread_cleanup_pop(0);
}


/* 클라이언트 스레드를 생성하는 함수 */
void start_client_thread(struct client_thread_info *cti)
{
	/* 스레드 구조체 초기화 */
	memset(&cti->thread, 0x00, sizeof (pthread_t));
	if (!pthread_create(&cti->thread, NULL, client_thread_loop, cti)) {
		cti->is_created = 1;
	} else {
		fprintf(stderr, "error creating client thread\n");
	}
}

int main(int argc, char **argv)
{
	struct client_thread_info client_threads[MAX_CLIENT];
	struct sockaddr_in server_addr, client_addr; 	// 소켓 주소 구조체 
	socklen_t client_addr_len; 
	int server_sockfd, client_sockfd;	// 소켓 디스크립터 
	char buf[BUFSIZE];
	int i;

	/* WiringPi 초기화 */
	wiringPiSetup();
	pinMode(LEDPIN, OUTPUT);

	/* 스레드 정보 초기화 */
	for (i=0; i<MAX_CLIENT; i++) {
		memset(client_threads, 0x00, sizeof (client_threads));
	}

	/* 서버 소켓 생성 */
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return -1;
	}

	/* 주소 구조체에 주소 지정 */
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(TCP_PORT);

	/* bind 함수를 사용하여 서버 소켓의 주소 설정 */
	if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("bind()");
		return -1;
	}

	/* 클라이언트 연결 대기 listen 시작 */
	if (listen(server_sockfd, MAX_CLIENT) < 0) {
		perror("listen()");
		return -1;
	}
	printf("listening on port %d ...\n", TCP_PORT);

	client_addr_len = sizeof(client_addr);

	/* 서버 loop 시작 */
	while (1) {
		/* 클라이언트가 접속하면 접속을 허용하고 클라이언트 소켓 생성 */
		client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (client_sockfd < 0) {
			perror("accept()");
			return -1;
		}
		
		/* sockfd가 유효하면 클라이언트 스레드 생성 */
		for (i=0; i<MAX_CLIENT; i++) {
			/* 빈 클라이언트 스레드 구조체를 찾아 생성 */
			if (!client_threads[i].is_created) {
				client_threads[i].client_addr = client_addr;
				client_threads[i].sockfd = client_sockfd; 
				start_client_thread(&client_threads[i]);
				break;
			}
		}
		
		if (i >= MAX_CLIENT) {
			fprintf(stderr, "cannot accept more client\n");
		}
	};

	return 0;
}
