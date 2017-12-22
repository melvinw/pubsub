#include <stdio.h>
#include <unistd.h>
#include "broker.h"
#include "message.h"
#include "common.h"

struct store_entry *sub_table = NULL;

int parse_msg(char* buf, struct Message* msg_p) {
    if (buf == NULL) {
        return -1;
    }
    
    sscanf(buf, "%d %s %s", (int*)&msg_p->message_type, msg_p->key, msg_p->value);
    return 0;
}

void process_publish(int fd, struct Message* msg_p) {
    struct store_entry* entry;    
    HASH_FIND_STR(sub_table, msg_p->key, entry);

    if (entry == NULL) {
	return;
    }
    
    struct sock_info* p = entry->sock_ptr;
    while (p) {
        //push key value information into the requested subscribes
        char buf[MAX_MSG_LEN];
        sprintf(buf, "%s %s", msg_p->key, msg_p->value);
	sendto(fd, buf, strlen(buf)+1, 0, (struct sockaddr*)&(p->sub_sock), p->sub_sock_len);
        p = p->next;
    }
}

void process_subscribe(struct Message* msg_p, struct sockaddr_un *from_p, socklen_t fromlen) {
    struct store_entry* entry;
    HASH_FIND_STR(sub_table, msg_p->key, entry);
    if (entry == NULL) {
        entry = (struct store_entry*)malloc(sizeof(struct store_entry));
        struct sock_info * sock_p = (struct sock_info*)malloc(sizeof(struct sock_info));
        memcpy(&sock_p->sub_sock, from_p, sizeof(struct sockaddr_un));
        sock_p->sub_sock_len = fromlen;
        sock_p->next = NULL;
        entry->sock_ptr = sock_p;
        strcpy(entry->key, msg_p->key);
        HASH_ADD_STR(sub_table, key, entry);
    }
    else {
	//check whether from_p exists in the linked list
        struct sock_info* p = entry->sock_ptr;
        while (p) {
            if (memcmp(from_p, &p->sub_sock, sizeof(struct sockaddr_un)) == 0) {
		break;
            }
            p = p->next;
        }
        if (p == NULL) {
            struct sock_info * sock_p = (struct sock_info*)malloc(sizeof(struct sock_info));
            memcpy(&sock_p->sub_sock, from_p, sizeof(struct sockaddr_un));
            sock_p->sub_sock_len = fromlen;
            sock_p->next = entry->sock_ptr->next;
            entry->sock_ptr->next = sock_p;
        }
    }
}

void process_unsubscribe(struct Message* msg_p, struct sockaddr_un *from_p, socklen_t fromlen) {
    struct store_entry* entry;
    HASH_FIND_STR(sub_table, msg_p->key, entry);
    if (entry == NULL) {
	return;
    }
    struct sock_info* prev = NULL; 
    struct sock_info* p = entry->sock_ptr;
    while (p) {
        if (memcmp(from_p, &p->sub_sock, sizeof(struct sockaddr_un)) == 0) {
	    break;
        }
      
        prev = p;
        p = p->next;
    }
   
    /*
     * if p is the only element, we also need to remove the entry from hash table
     * or else, we only need to remove the sock info from the linked list
     */ 
    if (p && prev) {
        prev->next = p->next;
        free(p);  
    }
    else if (p && !prev) {
	HASH_DEL(sub_table, entry);
    }
}

void msg_handler(int fd, char* buf, struct sockaddr_un *from_p, socklen_t fromlen) {
    struct Message msg;
    int ret;

    //todo check malloc fail 
    msg.key = (char*)malloc(KEY_LEN);
    msg.value = (char*)malloc(VALUE_LEN);
    ret = parse_msg(buf, &msg);
    if (ret < 0) {
        return;
    }
   
    if (msg.message_type == publish) {
        /*
         * Check the table, to see if there is any subscribe. 
         * If it is, send the key value info to the subsribes.
         */ 
        process_publish(fd, &msg); 
    }
    else if (msg.message_type == subscribe) {
        process_subscribe(&msg, from_p, fromlen);
    }
    else if (msg.message_type == unsubscribe) {
        process_unsubscribe(&msg, from_p, fromlen);
    } 

    free(msg.key);
    free(msg.value);
}

int main() {
    int fd;
    struct sockaddr_un addr;
    int ret;
    char buf[MAX_MSG_LEN];
    struct sockaddr_un from;
    int len;
    socklen_t fromlen = sizeof(from);
    volatile int running = 1;

    fd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (fd < 0) {
        printf("cannot create unix socket on broker\n");
	return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SERVER_SOCK_FILE);
    unlink(SERVER_SOCK_FILE);
    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        printf("unix bind failed on broker");
        return -1;	
    }

    while (running) {
	len = recvfrom(fd, buf, MAX_MSG_LEN, 0, (struct sockaddr *)&from, &fromlen);
 	if (len <= 0) {
            continue;
	}	
        
        printf("received buf:%s\n", buf); 
        msg_handler(fd, buf, &from, fromlen); 
      sleep(1);
    }

    if (fd > 0) {
	close(fd);
    }

    return 0;
}
