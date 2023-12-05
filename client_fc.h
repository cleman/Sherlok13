#ifndef CLIENT_FC
#define CLIENT_FC

int send_message(char *ip_addr, int portno, char *message);
void *server_tcp();
void create_table();
void print_playerValue();

#endif