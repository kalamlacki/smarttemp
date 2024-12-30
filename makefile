HEADERS = curlcommon.h esp8266httpreader.h dao.h sp1101w.h hs110.h tuya.h
OBJECTS = curlcommon.o esp8266httpreader.o dao.o sp1101w.o main.o hs110.o tuya.o

CPPFLAGS = -I/usr/include/libxml2 -I/usr/include/mariadb -I/usr/include/json-c -I/usr/include/python3.10 -I/usr/include/python3.11  -Wno-unused-result -Wsign-compare -g  -fstack-protector-strong -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O2 -Wall
LIBS = -L/usr/lib/x86_64-linux-gnu -lmariadb -lpthread -lz -lm -ldl -lcurl -lxml2 -ljson-c -lconfuse -lpython3.11 -lcrypt -lpthread -lutil

default: smarttemp

%.o: %.cpp $(HEADERS)
	g++ $(CPPFLAGS) -c $< -o $@

smarttemp: $(OBJECTS)
	g++ $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f $(OBJECTS)
	rm -f smarttemp
