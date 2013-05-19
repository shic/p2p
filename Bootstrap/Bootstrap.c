#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include<stdbool.h>

#include "Boot.h"

#define PORT 7500

#define BUFLEN 10000

#define BACKLOG       10

#define SERV_PORT   5194 //Possibile generare: errore in bind: Address already in use
#define MAXLINE     10240

#define MAX_STRING_LEN 8000

#define IP_LENGTH   30

#define MAXLINE_SINGLE_FILE     1024

char MSG[IP_LENGTH] ="";

int exitSign = 1;

//DA QUI£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡
void bootstrapRispondePeer() {

	struct file_struct *ptr = head;
	int count;
	for (count = 0; count < 2; count++) {
		ptr = ptr->next;
	}

	printf("\n [%s] \n", ptr->ip);

}

void display() {

	printf(" 1) conttata\n\n");
	printf(" 2) bootstrapRispondePeer\n\n");
	printf(" 3) \n\n");

	printf(" 0) ESCI\n\n");
	printf(" la tua scelta > ");

	// Aspetto scelta dell'utente
	int scelta;
	if (scanf("%i", &scelta) == 1) {
		scelta = (int) scelta;
	} else {
		printf("failed to read integer.\n");
	}

	if (scelta == 0) {
		// Stampo info di uscita
		exitSign = 0;
		printf("--------------------------------------\n");
		printf(" USCITO DAL PROGRAMMA\n");
		printf("--------------------------------------\n\n");
	} // ELSE
	else if (scelta == 1) {
		conttata();
	} else if (scelta == 2) {
		bootstrapRispondePeer();
	} else if (scelta == 3) {
	}

}

void conttata() {

	int listensd, connsd;
	struct sockaddr_in servaddr;

	char buff[MAXLINE];

	int bytecount;
	char MSG_ricevutoDaSuperpeer[MAXLINE];

	if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* crea il socket */
		perror("errore in socket");
		exit(1);
	}

	memset((void *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta
	 connessioni su una qualunque delle sue intefacce di rete */
	servaddr.sin_port = htons(SERV_PORT); /* numero di porta del server */

	//setsockopt(listensd,SOL_SOCKET,SO_REUSEADDR,(const char *)&opt,sizeof(int));
	//close(listensd);

	/* assegna l'indirizzo al socket */
	if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {
		perror("errore in bind");
		exit(1);
	}

	if (listen(listensd, BACKLOG) < 0) {
		perror("errore in listen");
		exit(1);
	}

	for (;;) {
		/* accetta una connessione con un client */

		if ((connsd = accept(listensd, (struct sockaddr *) NULL, NULL)) < 0) {
			perror("errore in accept");
			exit(1);
		}

		//receive the greeting
		bytecount = recv(connsd, buff, BUFLEN - 1, 0);
		// ssize_t recv (int __fd, void *__buf, size_t __n, int __flags);
		if (bytecount == -1) {
			perror("recv");
			exit(1);
		}

		buff[bytecount] = '\0';
		//MSG_ricevutoDaSuperpeer = buff;
		strcpy(MSG_ricevutoDaSuperpeer, buff);

		printf("MSG Received: \"%s\"\n", MSG_ricevutoDaSuperpeer);

		char newstr[MAX_STRING_LEN];
		int i;
		for (i = 8; MSG_ricevutoDaSuperpeer[i] != '\0'; i++) {
			newstr[i - 8] = MSG_ricevutoDaSuperpeer[i];
		}

		int j;
		for (j = 0; newstr[j] != '\0'; j++) {
			printf("IpofSuperpeer:  %c", newstr[j]);

		}

		add_to_list(newstr, true);
		print_list();

		//send the reply
		if (send(connsd, MSG, strlen(MSG) + 1, 0) == -1) {
			perror("send");
			exit(1);
		}

		if (close(connsd) == -1) { /* chiude la connessione */
			perror("errore in close");
			exit(1);
		}
	}
}

int main(int argc, char * argv[]) {

	while (exitSign) {
		display();
	}

	return 0;
}

