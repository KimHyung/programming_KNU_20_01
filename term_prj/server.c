#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

/* Client structure */
typedef struct{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
		arr[i-1] = '\0';
	    break;
    }
  }
}

void reset () {
  printf("\033[0m");
}

void color_switch (int i) {
	switch(i){
		case 0: printf("\033[1;31m");break;
		case 1: printf("\033[1;32m");break;
		case 2: printf("\033[1;33m");break;
		case 3: printf("\033[1;34m");break;
		case 4: printf("\033[1;35m");break;
		case 5: printf("\033[1;36m");break;
		case 6: printf("\033[7;31m");break;
		case 7: printf("\033[7;32m");break;
		case 8: printf("\033[7;33m");break;
		case 9: printf("\033[7;34m");break;
		default : reset();
	}
}

/* Add clients to queue */
void queue_add(client_t *cl){
	pthread_mutex_lock(&clients_mutex);
	for(int i=0; i < MAX_CLIENTS; ++i){
		if(!clients[i]){
			clients[i] = cl;
			break;
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}
/* Remove clients to queue */
void queue_remove(int uid){
	pthread_mutex_lock(&clients_mutex);
	for(int i=0; i < MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

//send_message except self
void send_message(char *s, int uid){
	char tmp;
	int len;
	pthread_mutex_lock(&clients_mutex);
	tmp = (uid%10)+'0';
	len = strlen(s);
	s[len-1] = tmp;
	strcat(s,"\n");
	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(write(clients[i]->sockfd, s, len+1) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

//send_message self
void send_message_self(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);
	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				if(write(clients[i]->sockfd, s, strlen(s)) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

void send_whisper_message(char *s, int uid){
	char name[10];
	char tmp_msg[BUFFER_SZ];
	int len;
	int i,j;
	int check=0;
	pthread_mutex_lock(&clients_mutex);
	if(s[1]=='w' && s[3]=='\"'){
		for(i=4,j=0;i<strlen(s);i++,j++){
			if(s[i]=='\"'){
				break;
			}
			name[j]=s[i];
		}
		len = i-j-1;
		for(i=4+len+2,j=0;i<strlen(s);i++,j++){
			if(s[i]=='\0')
				break;
			tmp_msg[j] = s[i];
			tmp_msg[j+1] = '-';
		}
		for(int i=0; i<MAX_CLIENTS;++i){
			if(clients[i]){
				if(strcmp(clients[i]->name,name)==0){
					if(write(clients[i]->sockfd, tmp_msg, strlen(tmp_msg))<0){
						printf("original: %s\n",s);
						perror("ERROR: write to descroptor failed");
						break;
					}
					check=1;
				}
			}
		}
		if(check == 0){
			s = "Wrong Name! \nUsage> /w \"name\" message_\n";
			len =strlen(s);
			for(int i=0; i<MAX_CLIENTS; ++i){
				if(clients[i]){
					if(clients[i]->uid == uid){
						if(write(clients[i]->sockfd, s, len)<0){
							perror("ERROR: wirte to descriptor failed");
							break;
						}
					}
				}
			}
		}
	}
	else{
		s = "Wrong command... \nUsage> /w \"name\" message_\n";
		len =strlen(s);
		for(int i=0; i<MAX_CLIENTS; ++i){
			if(clients[i]){
				if(clients[i]->uid == uid){
					if(write(clients[i]->sockfd, s, len)<0){
						perror("ERROR: wirte to descriptor failed");
						break;
					}
				}
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}



/* Handle all communication with the client */
void *handle_client(void *arg){
	char buff_out[BUFFER_SZ];
	char exit_code[BUFFER_SZ];
	char name[32];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

	// Name
	if(recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) <  2 || strlen(name) >= 32-1){
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	} else{
		for(int i=0; i<MAX_CLIENTS; ++i){
			if(clients[i]){
				if(strcmp(clients[i]->name,name)==0){
					strcpy(cli->name, name);
					send_message_self(buff_out, cli->uid);
					leave_flag = 1;
					break;
				}
			}
		}
		if (leave_flag){
			sprintf(exit_code, "__exit__");
			send_message_self(exit_code, cli->uid);
		}
		else{
			strcpy(cli->name, name);
			sprintf(buff_out, "%s has joined\n", cli->name);
			color_switch(cli->uid %10);
			printf("%s", buff_out);
			reset();
			send_message(buff_out, cli->uid);
		}
	}

	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				// printf("test: %c\n",buff_out[0]);
				if(buff_out[0] == '/'){
					send_whisper_message(buff_out, cli->uid);
				}
				else{
					send_message(buff_out, cli->uid);
					str_trim_lf(buff_out, strlen(buff_out));
					color_switch(cli->uid % 10);
					printf("%s\n", buff_out);
					reset();
				}
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			sprintf(buff_out, "%s has left\nNumber of Clients: %d \n", cli->name, cli_count);
			printf("%s", buff_out);
			send_message(buff_out, cli->uid);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

  /* Delete client from queue and thread */
	close(cli->sockfd);
  	queue_remove(cli->uid);
  	free(cli);
  	cli_count--;
  	pthread_detach(pthread_self());

	return NULL;
}

int main(int argc, char **argv){
	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]); //char* to int
	int option = 1;
	int server_sockfd = 0;
	int client_sockfd = 0;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	pthread_t tid;

	/* Socket settings */
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip);
	server_address.sin_port = htons(port);

 	 /* Ignore pipe signals */
	signal(SIGPIPE, SIG_IGN);

	if(setsockopt(server_sockfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
		perror("ERROR: Setsockopt failed");
    return EXIT_FAILURE;
	}

	/* Bind */
  	if(bind(server_sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
   		perror("ERROR: Socket binding failed");
    	return EXIT_FAILURE;
  	}

 	 /* Listen */
 	if (listen(server_sockfd, 10) < 0) {
		perror("ERROR: Socket listening failed");
		return EXIT_FAILURE;
	}

	printf("===============Chatting Room's Server==================\n");

	while(1){
		socklen_t clilent_len = sizeof(client_address);
		printf("Number of Clients: %d\n",cli_count);
		/*Wait until client connect to server*/
		client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_address, &clilent_len);

		/* Check Max*/
		if((cli_count + 1) == MAX_CLIENTS){
			printf("ERROR: Max clients reached.");
			close(client_sockfd);
			continue;
		}

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = client_address;
		cli->sockfd = client_sockfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &handle_client, (void*)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	return EXIT_SUCCESS;
}