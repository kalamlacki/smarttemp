#include <string.h>
#include <string>
#include <iostream>

#include <curl/curl.h> 
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "curlcommon.h"
#include "sp1101w.h"

using namespace std;

string sp1101wExtract(const char* buf, const char* xpath);
string get_value(xmlNodeSetPtr nodes);
string postHttpSP1101w(const char* url, const char* credentials, const char* postfields,  const char* xpath);

int readSP1101wStatus(const char* url, const char* credentials){
	string status = postHttpSP1101w(url, credentials, GETSTATUSXML,  SP1101WXPATH);
	if( status == "ON") {
		return 1;
	}
	else if ( status == "OFF" ) {
		return 0;
	}
	throw "Could not get status from SP-1101w";
}

int writeSP1101wStatus(const char* url, const char* credentials, int setStatus) {
	string status;
	if(setStatus == 1) {
		status = postHttpSP1101w(url, credentials, SETSTATUSON,  SP1101WXPATHCHECK);		
	}
	else if(setStatus == 0) {
		status = postHttpSP1101w(url, credentials, SETSTATUSOFF,  SP1101WXPATHCHECK);
	}
	else {
		throw "Unsupported argument";
	}
	if(status == "OK") {
		return 0;
	}
	throw "Could not set status of SP-1101w";
}

string postHttpSP1101w(const char* url, const char* credentials, const char* postfields,  const char* xpath){
	CURL *curl_handle;
	CURLcode res;
	string status;
	struct MemoryStruct chunk;

	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;
	 
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postfields);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
    curl_easy_setopt(curl_handle, CURLOPT_USERPWD, credentials);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 10l); 
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 20l); 
	
	res = curl_easy_perform(curl_handle);
	if(res != CURLE_OK) {
		cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
		status = curl_easy_strerror(res);
	}
	else {
		status=sp1101wExtract(chunk.memory, xpath);
	}
	curl_easy_cleanup(curl_handle);
	free(chunk.memory);
	return status;
}

string sp1101wExtract(const char* buf, const char* xpath) {
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj; 
   
    /* Load XML document */
	doc = xmlReadMemory(buf, strlen(buf), "noname.xml", NULL, 0);
    //doc = xmlParseFile(filename);
    if (doc == NULL) {
		string err = "Error: unable to parse content";
		cerr << err << buf << endl;
		return(err);
    }

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
		string err = "Error: unable to create new XPath context";
        cerr << err << endl;
        xmlFreeDoc(doc); 
        return(err);
    }
    
    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression((const xmlChar*)xpath, xpathCtx);
    if(xpathObj == NULL) {
		string err = "Error: unable to evaluate xpath expression";
        cerr << err << endl;
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        return(err);
    }
	
    string state = get_value(xpathObj->nodesetval);
	
    /* Cleanup */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 
    xmlFreeDoc(doc); 
    
    return state;
}

string get_value(xmlNodeSetPtr nodes) {
    xmlNodePtr cur;
    int size;
    string ret = "zonk";

    size = (nodes) ? nodes->nodeNr : 0;    
	if(size == 1 && nodes->nodeTab[0]->type == XML_ELEMENT_NODE) {
		ret = (const char*)xmlNodeGetContent(nodes->nodeTab[0]); 
    }
	return ret;
}