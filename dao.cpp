#include <stdlib.h>
#include <time.h> 
#include <string>
#include <sstream>
#include <mysql.h>
#include <stdio.h>
#include <confuse.h>
#include "dao.h"

static char* DBHOST = NULL;
static char* DBUSER = NULL;
static char* DBPASSWD = NULL;
static char* DBNAME = NULL;
static long DBPORT = 0;
static cfg_t *cfg;

using namespace std;

void readConfig(){
	cfg_opt_t opts[] = {
		CFG_SIMPLE_STR("dbhost", &DBHOST),
		CFG_SIMPLE_STR("dbuser", &DBUSER),
		CFG_SIMPLE_STR("dbpasswd", &DBPASSWD),
		CFG_SIMPLE_STR("dbname", &DBNAME),
		CFG_SIMPLE_INT("dbport", &DBPORT),
		CFG_END()
	};
	
	cfg = cfg_init(opts, 0);
	cfg_parse(cfg, "/etc/smarttemp.conf");
	
}

void cleanConfig() {
	cfg_free(cfg);
}

void finish_with_error(MYSQL *con){
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	throw "Database error occured";
}

void saveTemperatureToDB(const char* sernum, double temp, int status){
	MYSQL *con = mysql_init(NULL);
	if (con == NULL){
		throw "mysql_init() failed\n";
	}  
  
	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASSWD, DBNAME, (unsigned int)DBPORT, NULL, 0) == NULL){
		finish_with_error(con);
	}
	
	char date[20];
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
    timeinfo = localtime (&rawtime);
	strftime(date,20,"%F %T",timeinfo);
	
	ostringstream osquery;
    osquery << "insert into temperature_results(sernum, temp, readdate, user_id, smartplug_state) values ('" << sernum << "', " <<
		temp << ", '" << date << "', 3, " << status << ")";
	
	if (mysql_query(con, osquery.str().c_str())){
		finish_with_error(con);
	}
	mysql_close(con);
}

SmartPlugConfig getCurrentSettings(const char* sernum) {
	MYSQL *con = mysql_init(NULL);
	if (con == NULL){
		throw "mysql_init() failed\n";
	} 

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASSWD, DBNAME, (unsigned int)DBPORT, NULL, 0) == NULL){
		finish_with_error(con);
	}	
	
	ostringstream osquery;
    osquery << "select sernum, min_temp, max_temp, start_date, stop_date from temperature_schedule_view s where s.sernum='" << sernum << "'";
	
	if (mysql_query(con, osquery.str().c_str())){
		finish_with_error(con);
	}
	
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL){
		finish_with_error(con);
	}

	SmartPlugConfig spconf;
	int num_rows = mysql_num_rows(result);
	if(num_rows == 0) {
		mysql_free_result(result);
		mysql_close(con);
		throw "Config database (schedule) is misiing for this time!";
	}
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) { 
		spconf.minTemp = atof(row[1]);
		spconf.maxTemp = atof(row[2]);
		spconf.startDate = row[3];
		spconf.endDate = row[4];
		spconf.serialNo = row[0];
	}
  
    mysql_free_result(result);
	mysql_close(con);
	return spconf;
}

GlobalConfig** getGlobalSettings(int *size){
	MYSQL *con = mysql_init(NULL);
	if (con == NULL){
		throw "mysql_init() failed\n";
	} 

	if (mysql_real_connect(con, DBHOST, DBUSER, DBPASSWD, DBNAME, (unsigned int)DBPORT, NULL, 0) == NULL){
		finish_with_error(con);
	}	
	
	ostringstream osquery;
    osquery << "select sernum, url, mac, smart_plug_url, smart_plug_user_pass, \
		smart_plug_mac, enabled, smart_plug_type from serial_numbers";
	
	if (mysql_query(con, osquery.str().c_str())){
		finish_with_error(con);
	}
	
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL){
		finish_with_error(con);
	}
	
	int num_rows = mysql_num_rows(result);
	if(num_rows == 0) {
		mysql_free_result(result);
		mysql_close(con);
		throw "Config database (schedule) is misiing for this time!";
	}
	*size = num_rows;
	GlobalConfig** globalConfig = new GlobalConfig*[num_rows];
	for(int i=0; i < num_rows; i++) {
		globalConfig[i] = new GlobalConfig();
	}
	
	int i=0;
	MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) { 
		globalConfig[i]->serialNumber = row[0];
		globalConfig[i]->url = row[1];
		globalConfig[i]->mac = row[2] == NULL ? "0" : row[2];
		globalConfig[i]->smartPlugUrl = row[3] == NULL ? "0" : row[3];
		globalConfig[i]->smartPlugUserPass = row[4] == NULL ? "0" : row[4];
		globalConfig[i]->smartPlugMac = row[5] == NULL ? "0": row[5];
		globalConfig[i]->enabled = atoi( row[6] );
		globalConfig[i]->smartPlugType = row[7][0];
		i++;
	}
	
	mysql_free_result(result);
	mysql_close(con);
	return globalConfig;
}