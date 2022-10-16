#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/time.h>
#include <sys/socket.h>    //socket
#include <arpa/inet.h>     //inet_addr
#include <boost/algorithm/string.hpp>
#include <json.h>
#include <unistd.h>
#include "hs110.h"

#define BUFFER_SIZE 4048
#define MESSAGE_SIZE 4045
#define XOR_KEY 171

using namespace std;


string transmitHS110Command(const char* url, const char* message, char code);
string get_relay_state(const char* message);
string get_err_code(const char* message);

int readHS110Status(const char* url) {
	string status = transmitHS110Command(url, GETSTATUSJS, 0x21);
	if( status == "ON") {
		return 1;
	}
	else if ( status == "OFF" ) {
		return 0;
	}
	throw "Could not get status from HS110";
}

int writeHS110Status(const char* url, int setStatus) {
	string status;
	if(setStatus == 1) {
		status = transmitHS110Command(url, SETSTATUSONJS, 0x2d);		
	}
	else if(setStatus == 0) {
		status = transmitHS110Command(url, SETSTATUSOFFJS, 0x2d);
	}
	else {
		throw "Unsupported argument";
	}
	if(status == "OK") {
		return 0;
	}
	throw (string("Could not set status of HS110\n") + status).c_str();
}

string transmitHS110Command(const char* url, const char* message, char code) {
	char buffer[BUFFER_SIZE];
	char message_buff[MESSAGE_SIZE];
	int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) {
        throw "Could not create socket";
    }
	vector<string> url_elem;
	boost::split(url_elem, url, boost::is_any_of(":"));
     
    server.sin_addr.s_addr = inet_addr(url_elem[0].c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons( atoi(url_elem[1].c_str()) );
 
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		string err = "connect to HS110 failed. Error: ";
		err = err + strerror(errno);
		close(sock);
		throw err.c_str();
    }
    
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		string err = "connect to HS110 failed, SO_RCVTIMEO: ";
		err = err + strerror(errno);
		close(sock);
		throw err.c_str();
	}
	
	unsigned int size = xorEncrypt((unsigned char*)message, buffer, code); 
#ifdef DEBUG
	printf("po encrypt\n");
#endif
    if( send(sock, buffer, size, 0) < 0) {
		string err = "Send to HS110 failed. Error: ";
		err = err + strerror(errno);
		close(sock);
		throw err.c_str();
    }

#ifdef DEBUG
	printf("po send\n");
#endif    
	ssize_t rsize=0;
    if( (rsize = recv(sock , buffer , BUFFER_SIZE , 0)) < 0) {
		string err = "recv from HS110 failed. Error: ";
		err = err + strerror(errno);
		close(sock);
		throw err.c_str();        
    }
    close(sock);
	if(rsize == 0) {
		cout << "Zero response from socket plug!" << endl;
		return string("ZONK");
	}
#ifdef DEBUG
	cout << "po recv rsize=" << rsize <<endl;
	cout << (message_buff) << endl;
#endif	
	xorDecrypt((unsigned char*)buffer, message_buff, rsize);
#ifdef DEBUG
	printf("po decrypt\n");
	cout << (message_buff) << endl;
#endif	
	if(strcmp(message, GETSTATUSJS) == 0 ) {
		return get_relay_state(message_buff);
	}
	else {
		return get_err_code(message_buff);
	}
}

unsigned int xorEncrypt(const unsigned char* message, char* cryptogram, char code) {
	int m_length = strlen((char*)message);
	cryptogram[0] = '\0';
	cryptogram[1] = '\0';
	cryptogram[2] = '\0';
	cryptogram[3] = code;
	unsigned char key = XOR_KEY;
	for(int i=0; i<m_length; i++){
		key = key ^ message[i];
		cryptogram[i+4] = key;
	}
	return m_length + 4;
}

char* xorDecrypt(const unsigned char* cryptogram, char* message, unsigned int size) {
	unsigned char a, key = XOR_KEY;
	for(int i=4; i<size; i++){
		a = key ^ cryptogram[i];
		key = cryptogram[i];
		message[i-4] = a;
	}
	message[size-4] = '\0';
	return message;
}

string get_err_code(const char* message) {
	struct json_object *jobj, *found_key;
	jobj = json_tokener_parse(message);
	if(jobj == NULL) {
		return string("Parse json error in json_tokener_parse()");
	}
	if(json_object_object_get_ex(jobj, "system", &found_key)) {
		jobj = found_key;
		if(json_object_object_get_ex(jobj, "set_relay_state", &found_key)) {
			jobj = found_key;
			if(json_object_object_get_ex(jobj, "err_code", &found_key)) {
				const char* err_code = json_object_to_json_string_ext(found_key,0);
				if(atoi(err_code) == 0) {
					return string("OK");
				}
				else {
					return string("Smart plug err_code ") + err_code;
				}
			}
			else {
				return string("Did not find err_code!");
			}
		}
		else {
			return string("Did not find set_relay_state!");
		}
	}
	else {
		return string("Did not find system\n!");
	}
}

string get_relay_state(const char* message) {
	struct json_object *jobj, *found_key;
	jobj = json_tokener_parse(message);
	if(jobj == NULL) {
		return string("Parse json error in json_tokener_parse()");
	}

	if(json_object_object_get_ex(jobj, "system", &found_key)) {
		jobj = found_key;
		if(json_object_object_get_ex(jobj, "get_sysinfo", &found_key)) {
			jobj = found_key;
			if(json_object_object_get_ex(jobj, "err_code", &found_key)) {
				const char* err_code = json_object_to_json_string_ext(found_key,0);
				if(atoi(err_code) == 0) {
					if(json_object_object_get_ex(jobj, "relay_state", &found_key)){
						const char* relay_state = json_object_to_json_string_ext(found_key,0);
						int relay_s = atoi(relay_state);
						if(relay_s == 0) {
							return string("OFF");
						}
						else if (relay_s == 1) {
							return string("ON");
						}
						else {
							return string("WTF");
						}
					}
					else {
						return string("Did not find relay_state!");
					}
				}
				else {
					return string("Smart plug err_code ") + err_code;
				}
			}
			else {
				return string("Did not find err_code!");
			}
		}
		else {
			return string("Did not find get_sysinfo!");
		}
	}
	else {
		return string("Did not find system\n!");
	}
	
}