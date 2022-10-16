#include <stdio.h>
#include <iostream>

#include <curl/curl.h> 
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "esp8266httpreader.h"
#include "dao.h"
#include "sp1101w.h"
#include "hs110.h"
#include "tuya.h"

using namespace std;

int main(void){
	curl_global_init(CURL_GLOBAL_ALL);
	xmlInitParser();
	int size=0;
	GlobalConfig** globalConfig;
	try {
		readConfig();
		globalConfig = getGlobalSettings(&size);
		for(int i=0; i<size; i++ ) {
			if(globalConfig[i]->enabled == 1) {	
				try {
					double temp = readEsp8266Ds18b20Temp(globalConfig[i]->url.c_str());
					int status;
					if(globalConfig[i]->smartPlugType == 'E'){
						status = readSP1101wStatus(globalConfig[i]->smartPlugUrl.c_str(), globalConfig[i]->smartPlugUserPass.c_str());
					}
					else if(globalConfig[i]->smartPlugType == 'T') {
						status = readHS110Status(globalConfig[i]->smartPlugUrl.c_str());
					}
					else if(globalConfig[i]->smartPlugType == 'L') {
					    status = readTuyaStatus(globalConfig[i]->smartPlugUrl.c_str(), globalConfig[i]->smartPlugUserPass.c_str());
					}
					else {
						status = -1;
					}
					printf("Temperature: %.2f deg C, status: %d\n", temp, status);
					SmartPlugConfig spConf = getCurrentSettings(globalConfig[i]->serialNumber.c_str());
					printf("Settings are %s, %s, %s min: %.2f, max: %.2f\n", spConf.serialNo.c_str(), 
						spConf.startDate.c_str(), spConf.endDate.c_str(), spConf.minTemp, spConf.maxTemp);
					if(temp < spConf.minTemp && status == 0) {
						printf("Turning on SP-1101w plug for %s\n", globalConfig[i]->serialNumber.c_str());
						if(globalConfig[i]->smartPlugType == 'E'){
							writeSP1101wStatus(globalConfig[i]->smartPlugUrl.c_str(), globalConfig[i]->smartPlugUserPass.c_str(), 1);
						}
						else if(globalConfig[i]->smartPlugType == 'T') {
							writeHS110Status(globalConfig[i]->smartPlugUrl.c_str(), 1);
						}
						else if(globalConfig[i]->smartPlugType == 'L') {
							writeTuyaStatus(globalConfig[i]->smartPlugUrl.c_str(), globalConfig[i]->smartPlugUserPass.c_str(), 1);
						}
						status = 1;
						printf("SP-1101 for %s has been turned ON\n", globalConfig[i]->serialNumber.c_str());
					}
					else if(temp > spConf.maxTemp && status == 1 ){
						printf("Turning off SP-1101w plug\n");
						if(globalConfig[i]->smartPlugType == 'E'){
							writeSP1101wStatus(globalConfig[i]->smartPlugUrl.c_str(), globalConfig[i]->smartPlugUserPass.c_str(), 0);
						}
						else if(globalConfig[i]->smartPlugType == 'T') {
							writeHS110Status(globalConfig[i]->smartPlugUrl.c_str(), 0);
						}
						else if(globalConfig[i]->smartPlugType == 'L') {
							writeTuyaStatus(globalConfig[i]->smartPlugUrl.c_str(), globalConfig[i]->smartPlugUserPass.c_str(), 0);
						}
						status = 0;
						printf("SP-1101 for %s has been turned OFF\n", globalConfig[i]->serialNumber.c_str());
					}
					else {
						printf("I am not changing smartplug for %s\n", globalConfig[i]->serialNumber.c_str());
					}
					saveTemperatureToDB(globalConfig[i]->serialNumber.c_str(), temp, status);
				}catch(const char* e) {
					cerr << "Failed with:"<< globalConfig[i]->serialNumber.c_str() <<  " err: " << e << endl;
				}
			}
			else {
				printf("Config for %s has been disabled!\n", globalConfig[i]->serialNumber.c_str());
				
			}
		}
	} catch(const char* e) {
		cerr << "Program terminated wit err: " << e << endl;
	}
	for(int i=0; i < size; i++) {
		delete globalConfig[i];
	}
	if(size > 0) {
		delete globalConfig;
	}
	cleanConfig();
	xmlCleanupParser();
	xmlMemoryDump();
	curl_global_cleanup();
	return 0;
}
