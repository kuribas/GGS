/* tcps.c - a TCP server */

#include <linux/types.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <stdio.h>

main(int argc, char **argv)
{
    unsigned short port;       /* port server binds to                  */
    char buf[12];              /* buffer for sending and receiving data */
    struct sockaddr_in client; /* client address information            */
    struct sockaddr_in server; /* server address information            */
    int s;                     /* socket for accepting connections      */
    int ns;                    /* socket connected to client            */
    int namelen;               /* length of client name                 */

    /*
     * Check arguments. Should be only one: the port number to bind to.
     */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    /*
     * First argument should be the port.
     */
    port = (unsigned short) atoi(argv[1]);

    /*
     * Get a socket for accepting connections.
     */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit(2);

    /*
     * Bind the socket to the server address.
     */
    server.sin_family = AF_INET;
    server.sin_port   = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
       exit(3);

    /*
     * Listen for connections. Specify the backlog as 1.
     */
    if (listen(s, 1) != 0)
        exit(4);

    /*
     * Accept a connection.
     */
    namelen = sizeof(client);
    if ((ns = accept(s, (struct sockaddr *)&client, &namelen)) == -1)
        exit(5);

    /*
     * Receive the message on the newly connected socket.
     */
    if (recv(ns, buf, sizeof(buf), 0) == -1)
        exit(6);
    else 
    	printf ("Received: %s\n", buf);

    printf("Server ended successfully\n");
}
