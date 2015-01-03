/* tcpc.c - a TCP client */

#include <linux/types.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <netdb.h>
#include <stdio.h>

main(int argc, char **argv)
{
    unsigned short port;       /* port client will connect to              */
    char buf[12];              /* data buffer for sending and receiving    */
    struct hostent *hostnm;    /* server host name information             */
    struct sockaddr_in server; /* server address                           */
    int s;                     /* client socket                            */

    /*
     * Check Arguments Passed. Should be hostname and port.
     */
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    /*
     * The host name is the first argument. Get the server address.
     */
    hostnm = gethostbyname(argv[1]);
    if (hostnm == (struct hostent *) 0)
    {
        fprintf(stderr, "Gethostbyname failed\n");
        exit(2);
    }

    /*
     * The port is the second argument.
     */
    port = (unsigned short) atoi(argv[2]);

    /*
     * Put a message into the buffer.
     */
    strcpy(buf, "the message");

    /*
     * Put the server information into the server structure.
     * The port must be put into network byte order.
     */
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    /*
     * Get a stream socket.
     */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit(3);

    /*
     * Connect to the server.
     */
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
        exit(4);

    if (send(s, buf, sizeof(buf), 0) < 0)
        exit(5);
    else
    	printf("Sent: %s\n", buf);	 	

    printf("Client Ended Successfully\n");
}
