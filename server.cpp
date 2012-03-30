#include <iostream>
#include <sstream>
using namespace std;

#define _WINSOCKAPI_
#include <WinSock2.h>
#pragma comment (lib, "Ws2_32.lib")

#define SERVER_ADDRESS "192.168.1.2"
//#define SERVER_ADDRESS "93.181.195.49"

#define SERVER_PORT 25565
WSADATA Winsock;
SOCKET Socket;

sockaddr_in ServerAddress;
sockaddr_in IncomingAddress;
sockaddr_in ClientAddress[8];
char Buffer[1000];
int AddressLen = sizeof (IncomingAddress);

int main ()
{
	setlocale (LC_ALL, "Russian");
	WSAStartup (MAKEWORD (2, 2), &Winsock);
	if (LOBYTE (Winsock.wVersion) != 2 || HIBYTE (Winsock.wVersion) != 2)
	{
		WSACleanup ();
		return 1;
	}

	Socket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ZeroMemory (&ServerAddress, sizeof (ServerAddress));
	ServerAddress.sin_family = AF_INET;
	//ServerAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
	ServerAddress.sin_port = SERVER_PORT;
	bind (Socket, (sockaddr *)&ServerAddress, sizeof (ServerAddress));
	ZeroMemory (ClientAddress, sizeof (ClientAddress));

	cout << "Я - сервер" << endl;

	while (TRUE)
		if (recvfrom (Socket, Buffer, 1000, 0, (struct sockaddr *)&IncomingAddress, (int *)&AddressLen))
		{
			if (Buffer[0] == 1)
			{
				for (int i = 0; i < 8; i++)
					if (!ClientAddress[i].sin_family)
					{
						stringstream str;
						ClientAddress[i] = IncomingAddress;
						str << inet_ntoa (IncomingAddress.sin_addr) << ":" << IncomingAddress.sin_port << " подключился.";
						cout << str.str () << endl;
						for (int j = 0; j < 8; j++)
							if (ClientAddress[j].sin_family && (ClientAddress[j].sin_addr.s_addr != IncomingAddress.sin_addr.s_addr || ClientAddress[j].sin_port != IncomingAddress.sin_port))
								sendto (Socket, str.str ().c_str (), 1000, 0, (sockaddr *)&ClientAddress[j], sizeof (sockaddr));
						break;
					}
				continue;
			}

			cout << "Получено: " << Buffer << endl;

			if (Buffer[0] == ' ')
			{
				for (int i = 0; i < 8; i++)
					if (ClientAddress[i].sin_addr.s_addr == IncomingAddress.sin_addr.s_addr && ClientAddress[i].sin_port == IncomingAddress.sin_port)
							ZeroMemory (&ClientAddress[i], sizeof (sockaddr_in));
				stringstream str;
				str << inet_ntoa (IncomingAddress.sin_addr) << ":" << IncomingAddress.sin_port << " отключился.";
				cout << str.str () << endl;
				for (int i = 0; i < 8; i++)
						if (ClientAddress[i].sin_family)
							sendto (Socket, str.str ().c_str (), 1000, 0, (sockaddr *)&ClientAddress[i], sizeof (sockaddr));

				continue;
			}

			cout << "Передаю: " << Buffer << endl;
			for (int i = 0; i < 8; i++)
				if (ClientAddress[i].sin_family && (ClientAddress[i].sin_addr.s_addr != IncomingAddress.sin_addr.s_addr || ClientAddress[i].sin_port != IncomingAddress.sin_port))
					sendto (Socket, Buffer, 1000, 0, (sockaddr *)&ClientAddress[i], sizeof (sockaddr));
		}

	WSACleanup ();
	return 0;
}