#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

int main() {
	int s;

	// open a socket
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		return 1;
	}

	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	// bind the open socket to a port
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		printf("failed to bind to server port. exiting.\n");
		return 1;
	}

	listen(s, 10);
	printf("server started...\n");

	// loop start here

	// the last two params tell us who is connecting
	// accept will return the socket of the client
	int client_fd = accept(s, 0, 0);
	char buffer[256] = {0};
	recv(client_fd, buffer, 256, 0);
	// printf("%s\n", buffer);

	// http request will look like: GET /file.html ......
	// skip the "GET /"
	char* f = buffer + 5;

	// find the first space (after the file path)
	// set it to a null terminator
	*strchr(f, ' ') = 0;

	int opened_fd = open(f, O_RDONLY);

	char* message_okay = "HTTP/1.0 200 OK\r\n\r\n";

	// write and send (with 0 flags) are equivalent
	// write(client_fd, message_okay, strlen(message_okay));
	send(client_fd, message_okay, strlen(message_okay), 0);

	// send contents of the requested file
	long long len = 256;
	struct sf_hdtr hdtr;
	hdtr.hdr_cnt = 0;
	hdtr.trl_cnt = 0;
	sendfile(opened_fd, client_fd, 0, &len, &hdtr, 0);

	// close the file
	close(opened_fd);

	// close the client connection and the host socket
	close(client_fd);
	close(s);

	return 0;
}
