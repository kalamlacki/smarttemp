#include <string>

struct SmartPlugConfig {
  double minTemp;
  double maxTemp;
  std::string startDate;
  std::string endDate;
  std::string serialNo;
};

struct GlobalConfig {
  std::string serialNumber;
  std::string url;
  std::string mac;
  std::string smartPlugUrl;
  std::string smartPlugUserPass;
  std::string smartPlugMac;
  int enabled;
  char smartPlugType;
};

void saveTemperatureToDB(const char* sernum, double temp, int status);
SmartPlugConfig getCurrentSettings(const char* sernum);
GlobalConfig** getGlobalSettings(int* size);
void readConfig();
void cleanConfig();