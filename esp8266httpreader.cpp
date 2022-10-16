#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h> 
 
#include "curlcommon.h"
#include "esp8266httpreader.h"

 double readEsp8266Ds18b20Temp(const char* url){
	double temp;
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;
	 
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 10l); 
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 20l); 
	
	res = curl_easy_perform(curl_handle);
	if(res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
		curl_easy_cleanup(curl_handle);
		free(chunk.memory);	
		throw "Cannot communicate with esp8266Ds18B20 module";	
	}
	else {
		//printf("%lu bytes retrieved, content: %s\n", (long)chunk.size, chunk.memory);
		if(strncmp(chunk.memory, "DS18B20 Temperature=", strlen("DS18B20 Temperature=")) != 0) {
			curl_easy_cleanup(curl_handle);
			free(chunk.memory);
			throw "esp8266Ds18B20 module error";	
		}
		chunk.memory[chunk.size-1] = 0;
		temp=atof(chunk.memory+strlen("DS18B20 Temperature="));
	}
	curl_easy_cleanup(curl_handle);
	free(chunk.memory);
	return temp;
 } 