// SocketServer.cpp : Defines the entry point for the console application.
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms737593(v=vs.85).aspx

#include "stdafx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using namespace std;


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27022"

struct gsocket{
	bool active;
	SOCKET skt;
};
/*struct ThreadParams
{
SOCKET skt;
int i;
};*/

UINT __cdecl ServerThread(LPVOID pParam);
UINT __cdecl ClientThread(LPVOID pParam);

SOCKET server_socket;
gsocket my_socket[100];
bool recive_param;

int __cdecl main()
{
	int nRetCode = 0;

	printf("Starting program ... Thread ID: %u\n", GetCurrentThreadId());

	//int sn;
	for (int z01 = 0; z01 < 100; z01++)
		my_socket[z01].active = false;

	CWinThread* pServerThread = AfxBeginThread(ServerThread, 0);

	cout << "Press ESCAPE to terminate program\r\n";
	while (_getch() != 27);

	printf("\n\n");

	closesocket(server_socket);
	WSACleanup();

	return nRetCode;

	/*
	int sockfd = 0, n = 0;
	char recvBuff[1024];
	struct sockaddr_in serv_addr;

	memset(recvBuff, '0', sizeof(recvBuff));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	printf("\n Error : Could not create socket \n");
	return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
	printf("\n Error : Connect Failed \n");
	return 1;
	}

	while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0)
	{
	recvBuff[n] = 0;
	if (fputs(recvBuff, stdout) == EOF)
	{
	printf("\n Error : Fputs error");
	}
	printf("\n");
	}

	if (n < 0)
	{
	printf("\n Read Error \n");
	}
	*/

}

UINT __cdecl ServerThread(LPVOID pParam) {

	printf("Initializing server socket ... Thread ID: %u\n", GetCurrentThreadId());

	WSADATA wsa;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	struct addrinfo *result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
		printf("resolve the server address and port failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	// Create a SOCKET for connecting to server
	server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (server_socket == INVALID_SOCKET) {
		printf("Could not create socket: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	printf("Socket created.\n");

	struct sockaddr_in server, client;

	/*
	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	// Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
	printf("Bind failed with error code : %d", WSAGetLastError());
	}
	*/

	// Setup the TCP listening socket
	if (bind(server_socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	puts("Bind done");

	freeaddrinfo(result);

	// Listen to incoming connections
	// listen(s, 3);
	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	int addrlen = sizeof(struct sockaddr_in);

	// Accept a client socket
	//SOCKET client_socket;
	int sn;
	for (int z01 = 0; z01 < 100; z01++)
	if (!my_socket[z01].active) {
		sn = z01;
		z01 = 100;
	}

	while ((my_socket[sn].skt = accept(server_socket, (struct sockaddr *)&client, &addrlen)) != INVALID_SOCKET) {
		my_socket[sn].active = true;
		char *client_ip = inet_ntoa(client.sin_addr);
		int client_port = ntohs(client.sin_port);
		printf("incoming request from %s:%d\n", client_ip, client_port);
		recive_param = false;
		CWinThread *pClientThread = AfxBeginThread(ClientThread, (LPVOID)&sn);
		while (!recive_param)
			Sleep(100);
		for (int z01 = 0; z01 < 100; z01++)
		if (!my_socket[z01].active) {
			sn = z01;
			z01 = 100;
		}
	}
}

UINT __cdecl ClientThread(void *pParam)
{
	puts("Connection accepted");

	// No longer need server socket
	// closesocket(s);

	printf("Accepting client socket ... Thread ID: %u\n", GetCurrentThreadId());

	//SOCKET client_socket = (SOCKET)pParam;
	int *i = (int*)pParam;
	int sn = *i;
	recive_param = true;

	if (my_socket[sn].skt == INVALID_SOCKET) {
		printf("accept failed with error code: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	//Reply to the client		

	char buff[2048];
	strcpy_s(buff, "#Server Ready.\r\n");
	send(my_socket[sn].skt, buff, strlen(buff), 0);

	int number_of_bytes_received;
	while ((number_of_bytes_received = recv(my_socket[sn].skt, buff, sizeof(buff), 0)) != SOCKET_ERROR)
	{
		buff[number_of_bytes_received] = 0;
		puts(buff);
		for (int z01 = 0; z01 < 100; z01++)
		if (my_socket[z01].active)
			send(my_socket[z01].skt, buff, strlen(buff), 0);
	}

	int error = WSAGetLastError();
	switch (error)
	{
	case WSAEFAULT: // https://stackoverflow.com/questions/25194542/c-wsaefault-error-sending-integer
		printf("#%d: Bad address\n", error);
		break;
	case WSAECONNRESET:
		printf("#%d: Connection reset by peer\n", error);
		break;
	default:
		printf("#%d\n", error);
		break;
	}

	closesocket(my_socket[sn].skt);
	my_socket[sn].active = false;
	puts("Client socket closed!");
	return 0;
}

