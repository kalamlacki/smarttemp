#define SP1101WXPATH "//SMARTPLUG/CMD/Device.System.Power.State"
#define SP1101WXPATHCHECK "//SMARTPLUG/CMD" 
#define GETSTATUSXML "<?xml version=\"1.0\" ?><SMARTPLUG id=\"edimax\"><CMD id=\"get\"><Device.System.Power.State></Device.System.Power.State></CMD></SMARTPLUG>"
#define SETSTATUSON "<?xml version=\"1.0\" ?><SMARTPLUG id=\"edimax\"><CMD id=\"setup\"><Device.System.Power.State>ON</Device.System.Power.State></CMD></SMARTPLUG>"
#define SETSTATUSOFF "<?xml version=\"1.0\" ?><SMARTPLUG id=\"edimax\"><CMD id=\"setup\"><Device.System.Power.State>OFF</Device.System.Power.State></CMD></SMARTPLUG>"

int readSP1101wStatus(const char* url, const char* credentials);
int writeSP1101wStatus(const char* url, const char* credentials, int status);
