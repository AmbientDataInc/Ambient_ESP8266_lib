/*
 * ambient.h - Library for sending data to Ambient
 * Created by Takehiko Shimojima, April 21, 2016
 */
#ifndef Ambient_h
#define Ambient_h

#include "Arduino.h"
#if defined(ESP8266)
 #include <ESP8266WiFi.h>
#else
 #include <WiFi.h>
#endif

#define AMBIENT_WRITEKEY_SIZE 18
#define AMBIENT_MAX_RETRY 5
#define AMBIENT_DATA_SIZE 24
#define AMBIENT_NUM_PARAMS 11
#define AMBIENT_CMNT_SIZE 64
#define AMBIENT_TIMEOUT 3000 // milliseconds

class Ambient
{
public:

    Ambient(void);

    bool begin(unsigned int channelId, const char * writeKey, WiFiClient * c, int dev = 0);
    bool set(int field,const char * data);
	bool set(int field, double data);
	bool set(int field, int data);
    bool clear(int field);
    bool setcmnt(const char * cmnt);

    bool send( uint32_t tmout = 5000UL );
    int bulk_send(char * buf, uint32_t tmout = 5000UL);
    bool delete_data(const char * userKey);
    bool getchannel(const char * userKey, const char * devKey, unsigned int & channelId, char * writeKey, int len, WiFiClient * c, int dev = 0);
    int status;

private:

    WiFiClient * client;
    unsigned int channelId;
    char writeKey[AMBIENT_WRITEKEY_SIZE];
    int dev;
    char host[18];
    int port;

    struct {
        int set;
        char item[AMBIENT_DATA_SIZE];
    } data[AMBIENT_NUM_PARAMS];
    struct {
        int set;
        char item[AMBIENT_CMNT_SIZE];
    } cmnt;
};

#endif // Ambient_h
