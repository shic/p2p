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

#define IP_LENGTH   30

struct file_struct {
	char ip[IP_LENGTH];
	//char fileName[MAXLINE_SINGLE_FILE];

	struct file_struct *next;
};

struct file_struct *head = NULL;
struct file_struct *curr = NULL;

struct file_struct* create_list(char *ip) {
	printf("\n creating list with headnode as [%s]\n", ip);
	struct file_struct *ptr = (struct file_struct*) malloc(
			sizeof(struct file_struct));
	if (NULL == ptr) {
		printf("\n Node creation failed \n");
		return NULL;
	}
	char ipPointerToArray[IP_LENGTH];
	strcpy(ptr->ip, ip);
	//ptr->ip = ip;
	ptr->next = NULL;

	head = curr = ptr;
	return ptr;
}

struct file_struct* add_to_list(char *ip, bool add_to_end) {
	if (NULL == head) {
		return (create_list(ip));
	}

	if (add_to_end)
		printf("\n Adding node to end of list with value [%s]\n", ip);
	else
		printf("\n Adding node to beginning of list with value [%s]\n", ip);

	struct file_struct *ptr = (struct file_struct*) malloc(
			sizeof(struct file_struct));
	if (NULL == ptr) {
		printf("\n Node creation failed \n");
		return NULL;
	}
	strcpy(ptr->ip, ip);
	//ptr->ip = ip;
	ptr->next = NULL;

	if (add_to_end) {
		curr->next = ptr;
		curr = ptr;
	} else {
		ptr->next = head;
		head = ptr;
	}
	return ptr;
}

struct file_struct* search_in_list(char *ip, struct file_struct **prev) {
	struct file_struct *ptr = head;
	struct file_struct *tmp = NULL;
	bool found = false;

	printf("\n Searching the list for value [%s] \n", ip);

	while (ptr != NULL) {
		//if (ptr->ip == ip) {
		if (strcmp(ptr->ip, ip) == 0) {
			found = true;
			break;
		} else {
			tmp = ptr;
			ptr = ptr->next;
		}
	}

	if (true == found) {
		if (prev)
			*prev = tmp;
		return ptr;
	} else {
		return NULL;
	}
}

int delete_from_list(char *ip) {
	struct file_struct *prev = NULL;
	struct file_struct *del = NULL;

	printf("\n Deleting value [%s] from list\n", ip);

	del = search_in_list(ip, &prev);
	if (del == NULL)
	{
		return -1;
	} else {
		if (prev != NULL
		)
			prev->next = del->next;

		if (del == curr) {
			curr = prev;
		} else if (del == head) {
			head = del->next;
		}
	}

	free(del);
	del = NULL;

	return 0;
}

void print_list(void) {
	struct file_struct *ptr = head;

	printf("\n -------Printing list Start------- \n");
	while (ptr != NULL) {
		printf("\n [%s] \n", ptr->ip);
		ptr = ptr->next;
	}
	printf("\n -------Printing list End------- \n");
}


