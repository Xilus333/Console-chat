#include <iostream>
using namespace std;

#define _WINSOCKAPI_
#include <WinSock2.h>
#pragma comment (lib, "Ws2_32.lib")

#define SERVER_ADDRESS "169.254.100.47"
//#define SERVER_ADDRESS "93.181.195.49"

#define SERVER_PORT 25565
WSADATA Winsock;
SOCKET Socket;
struct sockaddr_in ServerAddress, InAddr;
char SendBuffer[1000], RecvBuffer[1000];
int SizeInt = sizeof (ServerAddress);

DWORD WINAPI RecvThread (LPVOID par);

int main ()
{
	setlocale (LC_ALL, "Russian");
	WSAStartup (MAKEWORD (2, 2), &Winsock);
	if (LOBYTE (Winsock.wVersion) != 2 || HIBYTE (Winsock.wVersion) != 2)
	{
		WSACleanup ();
		return 1;
	}

	char IP[50];
	cout << "Введите IP: ";
	cin >> IP;
	Socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ZeroMemory (&ServerAddress, sizeof (ServerAddress));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = inet_addr(IP); 
	ServerAddress.sin_port = SERVER_PORT;

	cout << "Я - клиент" << endl;

	CreateThread (NULL, 0, RecvThread, NULL, 0, NULL);

	SendBuffer[0] = 1;
	sendto (Socket, SendBuffer, 1, 0, (sockaddr *)&ServerAddress, sizeof (ServerAddress));

	while (TRUE)
	{
		cin.getline (SendBuffer, 1000);
		OemToCharA (SendBuffer, SendBuffer);
		if (strlen (SendBuffer) > 0)
		{
			sendto (Socket, SendBuffer, 1000, 0, (sockaddr *)&ServerAddress, sizeof (ServerAddress));
			if (SendBuffer[0] == ' ')
				break;
		}
	}

	WSACleanup ();
	return 0;
}

DWORD WINAPI RecvThread (LPVOID par)
{
	while (TRUE)
	{
		if (recvfrom (Socket, RecvBuffer, 1000, 0, (struct sockaddr *)&InAddr, &SizeInt))
			cout << "Получено: " << RecvBuffer << endl;
		if (SendBuffer[0] == ' ')
			break;
	}

	return 0;
}
//just for lulz