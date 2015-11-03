#include <windows.h>
#include <string>
#include <stdio.h>
#include <iostream>

using namespace std;
using std::string;

HINSTANCE hInst;
WSADATA wsaData;

void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename) {
	string::size_type n;
	string url = mUrl;
	if (url.substr(0,7) == "http://")
	        url.erase(0,7);
	if (url.substr(0,8) == "https://")
	        url.erase(0,8);
	n = url.find('/');
	if (n != string::npos) {
		serverName = url.substr(0,n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n+1);
	} else {
		serverName = url;
		filepath = "/";
		filename = "";
	}
}



SOCKET connectToServer(char *szServerName, WORD portNum) {
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;
	SOCKET conn;
	conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn == INVALID_SOCKET)
	        return NULL;
	if(inet_addr(szServerName)==INADDR_NONE) {
		hp=gethostbyname(szServerName);
	} else {
		addr=inet_addr(szServerName);
		hp=gethostbyaddr((char*)&addr,sizeof(addr),AF_INET);
	}
	if(hp==NULL) {
		closesocket(conn);
		return NULL;
	}
	server.sin_addr.s_addr=*((unsigned long*)hp->h_addr);
	server.sin_family=AF_INET;
	server.sin_port=htons(portNum);
	cout<<endl<<"Connecting to the internet..."<<endl;
	if(connect(conn,(struct sockaddr*)&server,sizeof(server))) {
		closesocket(conn);
		return NULL;
	}
	return conn;
}


int getHeaderLength(char *content) {
	const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
	char *findPos;
	int ofset = -1;
	findPos = strstr(content, srchStr1);
	if (findPos != NULL) {
		ofset = findPos - content;
		ofset += strlen(srchStr1);
	} else {
		findPos = strstr(content, srchStr2);
		if (findPos != NULL) {
			ofset = findPos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}


char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut) {
	const int bufSize = 512;
	char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
	char *tmpResult=NULL, *result;
	SOCKET conn;
	string server, filepath, filename;
	long totalBytesRead, thisReadSize, headerLen;
	mParseUrl(szUrl, server, filepath, filename);
	///////////// step 1, connect //////////////////////
	conn = connectToServer((char*)server.c_str(), 80);
	///////////// step 2, send GET request /////////////
	sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
	strcpy(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	sprintf(tmpBuffer, "Host: %s", server.c_str());
	strcat(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	strcat(sendBuffer, "\r\n");
	send(conn, sendBuffer, strlen(sendBuffer), 0);
	//    SetWindowText(edit3Hwnd, sendBuffer);
	//printf("Buffer being sent:\n%s", sendBuffer);
	///////////// step 3 - get received bytes ////////////////
	// Receive until the peer closes the connection
	totalBytesRead = 0;
	while(1) {
		memset(readBuffer, 0, bufSize);
		thisReadSize = recv (conn, readBuffer, bufSize, 0);
		if ( thisReadSize <= 0 )
		            break;
		tmpResult = (char*)realloc(tmpResult, thisReadSize+totalBytesRead);
		memcpy(tmpResult+totalBytesRead, readBuffer, thisReadSize);
		totalBytesRead += thisReadSize;
	}
	headerLen = getHeaderLength(tmpResult);
	long contenLen = totalBytesRead-headerLen;
	result = new char[contenLen+1];
	memcpy(result, tmpResult+headerLen, contenLen);
	result[contenLen] = 0x0;
	char *myTmp;
	myTmp = new char[headerLen+1];
	strncpy(myTmp, tmpResult, headerLen);
	myTmp[headerLen] = NULL;
	delete(tmpResult);
	*headerOut = myTmp;
	bytesReturnedOut = contenLen;
	closesocket(conn);
	return(result);
}


char glo[1000];
void functie(char * str) {
	int a=0;
	for (;str[a]!='"';a++) {
		cout<<str[a];
		glo[a]=str[a];
	}
	glo[a]=0;
}



bool verificare() {
	cout<<"Testing for updates...";
	char* temp; {
		char da[100];
		char szUrl[1000] = "http://isithackday.com/geoplanet-explorer/index.php?start=220097";
		long fileSize;
		char *memBuffer, *headerBuffer;
		memBuffer = headerBuffer = NULL;
		if ( WSAStartup(0x101, &wsaData) != 0)
		            return 0;
		memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
		//printf("data returned:\n%s", strstr(memBuffer,"data-country="));
		//cout<<headerBuffer<<endl;
		temp=strstr(memBuffer,"220097");
		if(!temp) {
			return 0;
		} else
		            temp=strstr(temp,"WOEID:");
		if(!temp) {
			return 0;
		}
		while(!(temp[0]>='0' and temp[0]<='9'))
		            strcpy(temp,temp+1);
		int a=0;
		while(temp[a]>='0' and temp[a]<='9')
		            a++;
		temp[a]=0;
		if (fileSize != 0) {
			delete(memBuffer);
			delete(headerBuffer);
		}
		WSACleanup();
	}
	//!!!!!!!!!!!!!!!!!!!!http://isithackday.com/geoplanet-explorer/index.php?start=220097!!!!!!!!!!!!!!!!!!!! {
		char szUrl[1000] = "http://weather.yahooapis.com/forecastrss?w=";
		strcat(szUrl,temp);
		strcat(szUrl,"&u=f");
		long fileSize;
		char *memBuffer, *headerBuffer;
		memBuffer = headerBuffer = NULL;
		if ( WSAStartup(0x101, &wsaData) != 0)
		            return 0;
		memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
		//printf("data returned:\n%s", strstr(memBuffer,"data-country="));
		char *temp;
		system("cls");
		cout<<"Country: ";
		functie(strstr(memBuffer,"country=")+1+strlen("country="));
		cout<<endl;
		cout<<"City: ";
		functie(strstr(memBuffer,"location city=")+1+strlen("location city="));
		cout<<endl;
		cout<<"Temperature now (F/C): ";
		functie(strstr(memBuffer,"temp=")+1+strlen("temp="));
		cout<<"/"<<(int)((atoi(glo)-32.0)/1.8);
		cout<<endl;
		cout<<"Humidify : ";
		functie(strstr(memBuffer,"humidity=")+1+strlen("humidity="));
		cout<<"%";
		cout<<endl;
		cout<<"Weather condition : ";
		functie(strstr(memBuffer,"text=")+1+strlen("text="));
		cout<<endl;
		cout<<"Sunrise  : ";
		functie(strstr(memBuffer,"sunrise=")+1+strlen("sunrise="));
		cout<<endl;
		cout<<"Sunset   : ";
		functie(strstr(memBuffer,"sunset=")+1+strlen("sunset="));
		cout<<endl;
		cout<<endl<<"Tomorrow : "<<endl;
		temp=strstr(memBuffer,":forecast");
		if(!temp) {
			return 0;
		}
		temp++;
		if(!temp) {
			return 0;
		}
		temp=strstr(temp,":forecast");
		if(!temp) {
			return 0;
		}
		temp++;
		cout<<"   ";
		functie(strstr(temp,"date=")+1+strlen("date="));
		cout<<endl;
		cout<<"   Min Temp (F/C): ";
		functie(strstr(temp,"low=")+1+strlen("low="));
		cout<<"/"<<(int)((atoi(glo)-32.0)/1.8);
		cout<<endl;
		cout<<"   Max Temp (F/C): ";
		functie(strstr(temp,"high=")+1+strlen("high="));
		cout<<"/"<<(int)((atoi(glo)-32.0)/1.8);
		cout<<endl;
		cout<<"   Weather condition: ";
		functie(strstr(temp,"text=")+1+strlen("text="));
		cout<<endl;
		// rain, status, sunrise and sunset and next day forecast and a check to make sure if the api get changed, It shows a message saying the program needs update.
		//        cout<<global;
		//        if(temp=strstr(memBuffer,"temp="))
		//        {
		//            for(int a=6;temp[a]!='"';a++)
		//                cout<<temp[a];
		//            cout<<"F";
		//        }
		if (fileSize != 0) {
			delete(memBuffer);
			delete(headerBuffer);
		}
		WSACleanup();
	}
	return 1;
}



int main() {
	if(!verificare()) {
		cout<<"Program needs to be updated or you dont have internet access."<<endl;
		system("pause");
		return 0;
	}
	start:;
	system("cls");
	char *temp; {
		char da[100];
		cout<<"Enter Zipcode:";
		cin.get();
		cin.getline(da,100);
		char szUrl[1000] = "http://isithackday.com/geoplanet-explorer/index.php?start=";
		char meh[100];
		strcpy(meh,da);
		for (int a=0;a<strlen(meh);a++)
		            if(meh[a]==' ')
		            meh[a]='+';
		strcat(szUrl,meh);
		long fileSize;
		char *memBuffer, *headerBuffer;
		memBuffer = headerBuffer = NULL;
		if ( WSAStartup(0x101, &wsaData) != 0)
		            return -1;
		memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
		//printf("data returned:\n%s", strstr(memBuffer,"data-country="));
		//cout<<headerBuffer<<endl;
		temp=strstr(memBuffer,da);
		if(!temp) {
			temp=strstr(headerBuffer,"woeid=");
			if(!temp) {
				cout<<endl<<"Invalid Zipcode/postal code.";
				Sleep(3000);
				goto start;
			}
		} else
		            temp=strstr(temp,"WOEID:");
		if(!temp) {
			cout<<endl<<"Invalid Zipcode/postal code.";
			Sleep(3000);
			goto start;
		}
		while(!(temp[0]>='0' and temp[0]<='9'))
		            strcpy(temp,temp+1);
		int a=0;
		while(temp[a]>='0' and temp[a]<='9')
		            a++;
		temp[a]=0;
		if (fileSize != 0) {
			delete(memBuffer);
			delete(headerBuffer);
		}
		WSACleanup();
	}
	//!!!!!!!!!!!!!!!!!!!!http://isithackday.com/geoplanet-explorer/index.php?start=220097!!!!!!!!!!!!!!!!!!!! {
		char szUrl[1000] = "http://weather.yahooapis.com/forecastrss?w=";
		strcat(szUrl,temp);
		strcat(szUrl,"&u=f");
		long fileSize;
		char *memBuffer, *headerBuffer;
		memBuffer = headerBuffer = NULL;
		if ( WSAStartup(0x101, &wsaData) != 0)
		            return -1;
		memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
		//printf("data returned:\n%s", strstr(memBuffer,"data-country="));
		char *temp;
		system("cls");
		cout<<"Country: ";
		functie(strstr(memBuffer,"country=")+1+strlen("country="));
		cout<<endl;
		cout<<"City: ";
		functie(strstr(memBuffer,"location city=")+1+strlen("location city="));
		cout<<endl;
		cout<<"Temperature now (F/C): ";
		functie(strstr(memBuffer,"temp=")+1+strlen("temp="));
		cout<<"/"<<(int)((atoi(glo)-32.0)/1.8);
		cout<<endl;
		cout<<"Humidify : ";
		functie(strstr(memBuffer,"humidity=")+1+strlen("humidity="));
		cout<<"%";
		cout<<endl;
		cout<<"Weather condition : ";
		functie(strstr(memBuffer,"text=")+1+strlen("text="));
		cout<<endl;
		cout<<"Sunrise  : ";
		functie(strstr(memBuffer,"sunrise=")+1+strlen("sunrise="));
		cout<<endl;
		cout<<"Sunset   : ";
		functie(strstr(memBuffer,"sunset=")+1+strlen("sunset="));
		cout<<endl;
		cout<<endl<<"Tomorrow : "<<endl;
		temp=strstr(memBuffer,":forecast");
		if(!temp) {
			cout<<endl<<"Invalid Zipcode/postal code.";
			Sleep(3000);
			goto start;
		}
		temp++;
		if(!temp) {
			cout<<endl<<"Invalid Zipcode/postal code.";
			Sleep(3000);
			goto start;
		}
		temp=strstr(temp,":forecast");
		if(!temp) {
			cout<<endl<<"Invalid Zipcode/postal code.";
			Sleep(3000);
			goto start;
		}
		temp++;
		cout<<"   ";
		functie(strstr(temp,"date=")+1+strlen("date="));
		cout<<endl;
		cout<<"   Min Temp (F/C): ";
		functie(strstr(temp,"low=")+1+strlen("low="));
		cout<<"/"<<(int)((atoi(glo)-32.0)/1.8);
		cout<<endl;
		cout<<"   Max Temp (F/C): ";
		functie(strstr(temp,"high=")+1+strlen("high="));
		cout<<"/"<<(int)((atoi(glo)-32.0)/1.8);
		cout<<endl;
		cout<<"   Weather condition: ";
		functie(strstr(temp,"text=")+1+strlen("text="));
		cout<<endl;
		// rain, status, sunrise and sunset and next day forecast and a check to make sure if the api get changed, It shows a message saying the program needs update.
		//cout<<global;
		//        if(temp=strstr(memBuffer,"temp="))
		//        {
		//            for(int a=6;temp[a]!='"';a++)
		//                cout<<temp[a];
		//            cout<<"F";
		//        }
		if (fileSize != 0) {
			delete(memBuffer);
			delete(headerBuffer);
		}
		WSACleanup();
	}
	cout<<endl<<endl;
	char ch[100];
	while(1) {
		cout<<"Press c to enter another zipcode or e to exit."<<endl;
		cin>>ch;
		strlwr(ch);
		if(ch[0]=='c' and ch[1]==0)
		            goto start;
		if(ch[0]=='e' and ch[1]==0)
		            break;
		Sleep(10);
	}
	return 0;
}
