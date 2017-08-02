// SocketClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define BUFFER_LENGTH 1024
#define ASSCII_ENTER 0xd

UINT __cdecl ServerThread(LPVOID pParam);
//int add(char* ch01);
int find(char* src, char* ID, char* value);

int main()
{

	char ID[1024];
	printf("enter your ID:");
	std::cin.getline(ID, 1000);
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char server_reply[2000];
	int recv_size;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(27022);

	//Connect to remote server
	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected");

	CWinThread* pServerThread = AfxBeginThread(ServerThread, (void *)s);

	char *message = "GET / HTTP/1.1\r\n\r\n";

	char buffer[BUFFER_LENGTH];
	ZeroMemory(buffer, BUFFER_LENGTH);
	int i = 0;

	memcpy(buffer + i, "#ID:", 4);
	i += 4;
	memcpy(buffer + i, ID, strlen(ID));
	i += strlen(ID);
	memcpy(buffer + i, "#MSG:", 5);
	i += 5;

	int c;
	//i = 0;
	bool ignore = false;

	do
	{
		c = _getch();

		if (ignore)
		{
			if (isprint(c))
			{
				printf("[%c]", c);
			}
			else
			{
				printf("[0x%x]", c);
			}
			ignore = false;
			continue;
		}

		if (isprint(c))
		{
			buffer[i] = c;
			printf("%c", c);
			i++;
		}
		else if (iscntrl(c))
		{
			printf("[ctrl: 0x%x]", c);

			if (c == 0x0)
				ignore = true;

			if (c == ASSCII_ENTER)
			{
				printf("\n");
				buffer[i] = '\0';
				i = strlen(ID) + 9;// 0;

				printf("buffer: [%s]\n", buffer);
				if (send(s, buffer, strlen(buffer), 0) < 0)//if (send(s, buffer, strlen(message), 0) < 0)
				{
					puts("Send failed");
					return 1;
				}

				/*
				//Receive a reply from the server
				if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR)
				{
				puts("recv failed");
				}
				puts("Reply received\n");

				// Add a NULL terminating character to make it a proper string before printing
				server_reply[recv_size] = '\0';
				puts(server_reply);
				*/
			}
		}
		else
		{
			printf("[0x%x]", c);

			if (c == 0xe0)
				ignore = true;
		}

	} while (c != 27);

	closesocket(s);
	puts("Socket closed.");

	WSACleanup();
	puts("WSA Cleanuped.");

	/*
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;

	char sendBuff[1025];
	int numrv;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("socket retrieve success\n");

	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (listen(listenfd, 10) == -1) {
	printf("Failed to listen\n");
	return -1;
	}

	while (1)
	{
	connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); // accept awaiting request

	strcpy(sendBuff, "Message from server");
	write(connfd, sendBuff, strlen(sendBuff));

	close(connfd);
	sleep(1);
	}
	*/

	return 0;
}

UINT __cdecl ServerThread(LPVOID pParam)
{
	SOCKET client_socket = (SOCKET)pParam;
	int number_of_bytes_received;
	char buff[2048];
	while ((number_of_bytes_received = recv(client_socket, buff, sizeof(buff), 0)) != SOCKET_ERROR)
	{
		buff[number_of_bytes_received] = 0;
		char id[1024], msg[1024];
		if (find(buff, "MSG", msg) == 0 && find(buff, "ID", id) == 0)
			printf("%s: [%s]\n", id, msg);
		else puts(buff);
		//send(client_socket, buff, strlen(buff), 0);
	}

	return 0;

	//???? ???? ?? client socket ? close ????

}

int find(char* src, char* ID, char* value)
{
	unsigned char i[20];
	unsigned char count = 0;

	//find # symbol
	int z01, z02, z03;
	for (z01 = 0; z01 < strlen(src); z01++)
	{
		if (src[z01] == '#') {
			i[count] = z01;
			count++;
		}
	}
	//check if after symbol # equal to Id
	bool equal = true;
	for (z01 = 0; z01 < count; z01++)
	{
		equal = true;
		for (z02 = 0; z02 < strlen(ID); z02++)
		{
			if (src[i[z01] + z02 + 1] != ID[z02]){
				equal = false;
				z02 = strlen(ID);
			}
		}
		if (equal) {
			//get value
			if (z01 + 1 == count){
				memcpy(value, src + i[z01] + strlen(ID) + 2, strlen(src) - i[z01] - strlen(ID) - 2);
				value[strlen(src) - i[z01] - strlen(ID) - 2] = 0;
			}
			else{
				memcpy(value, src + i[z01] + strlen(ID) + 2, i[z01 + 1] - i[z01] - strlen(ID) - 2);
				value[i[z01 + 1] - i[z01] - strlen(ID) - 2] = 0;
			}
			//value 
			z01 = count;
		}
	}

	if (!equal) return 1;

	return 0;
}