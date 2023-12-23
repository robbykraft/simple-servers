#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1000000

void make_http_response(
	const char *file_name,
	// const char *file_ext,
	char *response,
	size_t *response_length) {
	// const char *mime_type = get_mime_type(file_ext);
	char *header = (char*)malloc(BUFFER_SIZE * sizeof(char));
	snprintf(header, BUFFER_SIZE,
		"HTTP/1.0 200 OK\r\n"
		"Content-Type: %s\r\n"
		"\r\n",
		"text/plain");

	int file_fd = open(file_name, O_RDONLY);
	if (file_fd == -1) {
		snprintf(response, BUFFER_SIZE,
			"HTTP/1.1 404 Not Found\r\n"
			"Content-Type: text/plain\r\n"
			"\r\n"
			"404 Not Found");
		*response_length = strlen(response);
		return;
	}

	struct stat file_stat;
	fstat(file_fd, &file_stat);
	off_t file_size = file_stat.st_size;

	*response_length = 0;
	memcpy(response, header, strlen(header));
	*response_length += strlen(header);

	ssize_t bytes_read;
	while((bytes_read = read(file_fd,
		response + *response_length,
		BUFFER_SIZE - *response_length)) > 0) {
		*response_length += bytes_read;
	}
	free(header);
	close(file_fd);
}

int main() {
	int server_fd;

	// open a socket
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		return 1;
	}

	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	// bind the open socket to a port
	if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
		printf("failed to bind to server port. exiting.\n");
		return 1;
	}

	listen(server_fd, 10);
	printf("server started...\n");

	// loop start here

	while(1) {
		// the last two params tell us who is connecting
		// accept will return the socket of the client
		int client_fd;
		if ((client_fd = accept(server_fd, 0, 0)) < 0) {
			printf("client connection failed\n");
			// continue;
			return 1;
		}
		char buffer[256] = {0};
		recv(client_fd, buffer, 256, 0);
		// printf("%s\n", buffer);

		// http request will look like: GET /file.html ......
		// skip the "GET /"
		char* f = buffer + 5;

		// find the first space (after the file path)
		// set it to a null terminator
		*strchr(f, ' ') = 0;

		char *response = (char*)malloc(BUFFER_SIZE * sizeof(char) * 2);
		size_t response_length;

		make_http_response(f, response, &response_length);
		send(client_fd, response, response_length, 0);
		free(response);

		// close the client connection and the host socket
		close(client_fd);
	}

	close(server_fd);

	return 0;
}
