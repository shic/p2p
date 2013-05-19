#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <fcntl.h>
#include <time.h>

#include <ifaddrs.h>

#include <stropts.h>
//#include <linux/netdevice.h>

#define BACKLOG       10

#define SERV_PORT   5194
#define MAXLINE     10240

#define IP_LENGTH   30

#define PORT 7500

#define BUFLEN 10000

#define MAX_STRING_LEN 8000

char MSG_QUERY[IP_LENGTH+10] = "/QUERY";


void display();

void getIpLocale();

void superpeerContattaBootstrap(char*, char*);
void gestionRichiestaJoin(); //server

char ipDestinatarioSceltoDalSistema[MAX_STRING_LEN] = "127.0.0.1";
char ipLocale[INET_ADDRSTRLEN];

int exitSign = 1;

int main() {
	getIpLocale();

	while (exitSign) {
		display();
	}
	return 0;
}

void display() {

	printf(" 1) superpeerContattaBootstrap(client)\n\n");
	printf(" 2) receiveFileP2P(client)\n\n");
	printf(" 3) gestionRichiestaJoin\n\n");

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
		superpeerContattaBootstrap(ipLocale, ipDestinatarioSceltoDalSistema);
	} else if (scelta == 2) {
		//	receiveFileP2P(ipLocale, ipDestinatarioSceltoDalSistema);
	} else if (scelta == 3) {
		gestionRichiestaJoin(ipLocale, ipDestinatarioSceltoDalSistema);
	}

}

void superpeerContattaBootstrap(char* ipLocale, char* ipDestinatario) {
//CLIENT
	int sockfd;
	struct sockaddr_in servaddr;
	int bytecount;
	char buff[MAXLINE];

	// concatena la stringa a con "&&"
	strcat(MSG_QUERY, "&&");

	// concatena la stringa MSG_QUERY con ipLocalePeer
	strcat(MSG_QUERY, ipLocale);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* crea il socket */
		perror("errore in socket");
		exit(1);
	}

	memset((void *) &servaddr, 0, sizeof(servaddr)); /* azzera servaddr */
	servaddr.sin_family = AF_INET; /* assegna il tipo di indirizzo */
	servaddr.sin_port = htons(SERV_PORT); /* assegna la porta del server */
	/* assegna l'indirizzo del server prendendolo dalla riga di comando.
	 L'indirizzo è una stringa e deve essere convertito in intero in
	 network byte order. */

	if (inet_pton(AF_INET, ipDestinatario, &servaddr.sin_addr) <= 0) {
		fprintf(stderr, "errore in inet_pton per %s\n", ipDestinatario);
		exit(1);
	}

	/* stabilisce la connessione con il server */
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		perror("errore in connect");
		exit(1);
	}

	if (send(sockfd, MSG_QUERY, strlen(MSG_QUERY) + 1, 0) == -1) {
		perror("send");
		exit(1);
	}
	//send the reply
	if (send(sockfd, ipLocale, strlen(ipLocale) + 1, 0) == -1) {
		perror("send");
		exit(1);
	}

	//receive the message from bootstrap
	bytecount = recv(sockfd, buff, BUFLEN - 1, 0);
	if (bytecount == -1) {
		perror("recv");
		exit(1);
	}

	buff[bytecount] = '\0';
	printf("MSG Received from Boostrap: \"%s\"\n", buff);
}

void getIpLocale() {

	int s;
	struct ifconf ifconf;
	struct ifreq ifr[50];
	int ifs;
	int i;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
	}

	ifconf.ifc_buf = (char *) ifr;
	ifconf.ifc_len = sizeof ifr;

	if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) {
		perror("ioctl");
	}
	ifs = ifconf.ifc_len / sizeof(ifr[0]);

	for (i = 0; i < ifs; i++) {
		struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;

		if (!inet_ntop(AF_INET, &s_in->sin_addr, ipLocale, sizeof(ipLocale))) {
			perror("inet_ntop");
		}
		//   printf("%s \n" ,ip);
	}
	close(s);
}

void gestionRichiestaJoin() {
	//SERVER
	int listensd, connsd;
	struct sockaddr_in servaddr;
	char buff[MAXLINE];
	char MSG[] = "JoinOK";

	char MSG_ricevutoDaSuperpeer[MAXLINE];
	int bytecount;

	if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* crea il socket */
		perror("errore in socket");
		exit(1);
	}

	memset((void *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* il server accetta
	 connessioni su una qualunque delle sue intefacce di rete */
	servaddr.sin_port = htons(SERV_PORT); /* numero di porta del server */

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
		//receive message
		bytecount = recv(connsd, buff, BUFLEN - 1, 0);
		// ssize_t recv (int __fd, void *__buf, size_t __n, int __flags);
		if (bytecount == -1) {
			perror("recv");
			exit(1);
		}

		buff[bytecount] = '\0';
		//MSG_ricevutoDaSuperpeer = buff;
		strcpy(MSG_ricevutoDaSuperpeer, buff);

		printf("MSG Received from peer: \"%s\"\n", MSG_ricevutoDaSuperpeer);

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

