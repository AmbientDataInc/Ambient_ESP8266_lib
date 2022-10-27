/*
 * ambient.cpp - Library for sending data to Ambient
 * Created by Takehiko Shimojima, April 21, 2016
 */
#include "Ambient.h"

#define AMBIENT_DEBUG 0

#if AMBIENT_DEBUG
#define DBG(...) { Serial.print(__VA_ARGS__); }
#define ERR(...) { Serial.print(__VA_ARGS__); }
#else
#define DBG(...)
#define ERR(...)
#endif /* AMBIENT_DBG */

// const char* AMBIENT_HOST = "54.65.206.59";
const char* AMBIENT_HOST = "ambidata.io";
int AMBIENT_PORT = 80;
const char* AMBIENT_HOST_DEV = "192.168.11.2";
int AMBIENT_PORT_DEV = 4567;

const char * ambient_keys[] = {"\"d1\":\"", "\"d2\":\"", "\"d3\":\"", "\"d4\":\"", "\"d5\":\"", "\"d6\":\"", "\"d7\":\"", "\"d8\":\"", "\"lat\":\"", "\"lng\":\"", "\"created\":\""};

Ambient::Ambient() {
}

bool
Ambient::begin(unsigned int channelId, const char * writeKey, WiFiClient * c, const char * readKey, int dev) {
    this->channelId = channelId;

    if (sizeof(writeKey) > AMBIENT_WRITEKEY_SIZE) {
        ERR("writeKey length > AMBIENT_WRITEKEY_SIZE");
        return false;
    }
    strcpy(this->writeKey, writeKey);

    if(NULL == c) {
        ERR("Socket Pointer is NULL, open a socket.");
        return false;
    }
    this->client = c;
    if (readKey != NULL) {
        strcpy(this->readKey, readKey);
    } else {
        strcpy(this->readKey, "");
    }
    this->dev = dev;
    if (dev) {
        strcpy(this->host, AMBIENT_HOST_DEV);
        this->port = AMBIENT_PORT_DEV;
    } else {
        strcpy(this->host, AMBIENT_HOST);
        this->port = AMBIENT_PORT;
    }
    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        this->data[i].set = false;
    }
    this->cmnt.set = false;
    this->lastsend = 0;
    return true;
}

bool
Ambient::set(int field,const char * data) {
    --field;
    if (field < 0 || field >= AMBIENT_NUM_PARAMS) {
        return false;
    }
    if (strlen(data) > AMBIENT_DATA_SIZE) {
        return false;
    }
    this->data[field].set = true;
    strcpy(this->data[field].item, data);

    return true;
}

bool Ambient::set(int field, double data)
{
	return set(field,String(data).c_str());
}

bool Ambient::set(int field, int data)
{
	return set(field, String(data).c_str());
}

bool
Ambient::clear(int field) {
    --field;
    if (field < 0 || field >= AMBIENT_NUM_PARAMS) {
        return false;
    }
    this->data[field].set = false;
    this->cmnt.set = false;

    return true;
}

bool
Ambient::setcmnt(const char * data) {
    if (strlen(data) > AMBIENT_CMNT_SIZE) {
        return false;
    }
    this->cmnt.set = true;
    strcpy(this->cmnt.item, data);

    return true;
}

bool Ambient::connect2host(uint32_t tmout) {
    int retry;
    for (retry = 0; retry < AMBIENT_MAX_RETRY; retry++) {
        int ret;
#if defined(ESP8266)
        this->client->setTimeout(tmout);
        ret = this->client->connect(this->host, this->port);
#else
        ret = this->client->connect(this->host, this->port, tmout);
#endif
        if (ret) {
            break ;
        }
    }
    if(retry == AMBIENT_MAX_RETRY) {
        ERR("Could not connect socket to host\r\n");
        return false;
    }
    return true;
}

#define GetStatMaxRetry 100

int
Ambient::getStatusCode() {
    String _buf;
    int retry = 0;
    while (retry < GetStatMaxRetry) {
        _buf = this->client->readStringUntil('\n');
        if (_buf.length() != 0) {
            break;
        }
        delay(10);
        retry++;
    }
    if (retry != 0) {
        DBG("get stat retry:"); DBG(retry); DBG("\r\n");
        ;
    }
    if (retry == GetStatMaxRetry) {
        return 0;
    }
    int from = _buf.indexOf("HTTP/1.1 ") + sizeof("HTTP/1.1 ") - 1;
    int to = _buf.indexOf(' ', from);
    this->status = _buf.substring(from, to).toInt();
    return this->status;
}

bool
Ambient::send(uint32_t tmout) {
    char str[180];
    char body[192];
    char inChar;

    if (this->lastsend != 0 && (millis() - this->lastsend) < 4999) {
        this->status = 403;
        return false;
    }
    this->status = 0;
    if (connect2host(tmout) == false) {
        return false;
    }
    memset(body, 0, sizeof(body));
    strcat(body, "{\"writeKey\":\"");
    strcat(body, this->writeKey);
    strcat(body, "\",");

    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        if (this->data[i].set) {
            strcat(body, ambient_keys[i]);
            strcat(body, this->data[i].item);
            strcat(body, "\",");
        }
    }
    if (this->cmnt.set) {
        strcat(body, "\"cmnt\":\"");
        strcat(body, this->cmnt.item);
        strcat(body, "\",");
    }
    body[strlen(body) - 1] = '\0';
    strcat(body, "}\r\n");

    memset(str, 0, sizeof(str));
    sprintf(str, "POST /api/v2/channels/%u/data HTTP/1.1\r\n", this->channelId);
    if (this->port == 80) {
        sprintf(&str[strlen(str)], "Host: %s\r\n", this->host);
    } else {
        sprintf(&str[strlen(str)], "Host: %s:%d\r\n", this->host, this->port);
    }
    sprintf(&str[strlen(str)], "Content-Length: %d\r\n", strlen(body));
    sprintf(&str[strlen(str)], "Content-Type: application/json\r\n\r\n");

    DBG("sending: ");DBG(strlen(str));DBG("bytes\r\n");DBG(str);

    int ret;
    ret = this->client->print(str);
    delay(30);
    DBG(ret);DBG(" bytes sent\n\n");
    if (ret == 0) {
        ERR("send failed\n");
        return false;
    }
    ret = this->client->print(body);
    delay(30);
    DBG(ret);DBG(" bytes sent\n\n");
    if (ret == 0) {
        ERR("send failed\n");
        return false;
    }

    getStatusCode();
    while (this->client->available()) {
        inChar = this->client->read();
#if AMBIENT_DEBUG
        Serial.write(inChar);
#endif
    }

    this->client->stop();

    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        this->data[i].set = false;
    }
    this->cmnt.set = false;
    this->lastsend = millis();

    return true;
}

int
Ambient::bulk_send(char *buf, uint32_t tmout) {
    char str[180];
    char inChar;

    if (this->lastsend != 0 && (millis() - this->lastsend) < 4999) {
        this->status = 403;
        return false;
    }
    this->status = 0;
    if (connect2host(tmout) == false) {
        return false;
    }
    memset(str, 0, sizeof(str));
    sprintf(str, "POST /api/v2/channels/%u/dataarray HTTP/1.1\r\n", this->channelId);
    if (this->port == 80) {
        sprintf(&str[strlen(str)], "Host: %s\r\n", this->host);
    } else {
        sprintf(&str[strlen(str)], "Host: %s:%d\r\n", this->host, this->port);
    }
    sprintf(&str[strlen(str)], "Content-Length: %d\r\n", strlen(buf));
    sprintf(&str[strlen(str)], "Content-Type: application/json\r\n\r\n");

    DBG("sending: ");DBG(strlen(str));DBG("bytes\r\n");DBG(str);

    int ret;
    ret = this->client->print(str); // send header
    delay(30);
    DBG(ret);DBG(" bytes sent\n\n");
    if (ret == 0) {
        ERR("send failed\n");
        return -1;
    }

    int sent = 0;
    unsigned long starttime = millis();
    while ((millis() - starttime) < AMBIENT_TIMEOUT) {
        ret = this->client->print(&buf[sent]);
        delay(30);
        DBG(ret);DBG(" bytes sent\n\n");
        if (ret == 0) {
            ERR("send failed\n");
            return -1;
        }
        sent += ret;
        if (sent >= strlen(buf)) {
            break;
        }
    }
    delay(500);

    getStatusCode();
    while (this->client->available()) {
        inChar = this->client->read();
#if AMBIENT_DEBUG
        Serial.write(inChar);
#endif
    }

    this->client->stop();

    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        this->data[i].set = false;
    }
    this->cmnt.set = false;
    this->lastsend = millis();

    return (sent == 0) ? -1 : sent;
}

bool
Ambient::read(char * buf, int len, int n, uint32_t tmout) {
    char str[180];
    String _buf;

    this->status = 0;
    if (connect2host(tmout) == false) {
        return false;
    }
    memset(str, 0, sizeof(str));
    sprintf(str, "GET /api/v2/channels/%u/data?readKey=%s&n=%d HTTP/1.1\r\n", this->channelId, this->readKey, n);
    if (this->port == 80) {
        sprintf(&str[strlen(str)], "Host: %s\r\n\r\n", this->host);
    } else {
        sprintf(&str[strlen(str)], "Host: %s:%d\r\n\r\n", this->host, this->port);
    }

    DBG("sending: ");DBG(strlen(str));DBG("bytes\r\n");DBG(str);

    int ret;
    ret = this->client->print(str);
    delay(30);
    DBG(ret);DBG(" bytes sent\n\n");
    if (ret == 0) {
        ERR("send failed\n");
        return false;
    }

    if (getStatusCode() != 200) {  // ステータスコードが200でなければ
        while (this->client->available()) {    // 残りを読み捨てる
            this->client->readStringUntil('\n');
        }
        return false;
    }
    while (this->client->available()) {
        _buf = this->client->readStringUntil('\n');
        if (_buf.length() == 1)  // 空行を見つける
            break;
    }
    _buf = this->client->readStringUntil('\n');
    _buf.toCharArray(buf, len);

    this->client->stop();

    return true;
}

bool
Ambient::delete_data(const char * userKey, uint32_t tmout) {
    char str[180];
    char inChar;

    this->status = 0;
    if (connect2host(tmout) == false) {
        return false;
    }
    memset(str, 0, sizeof(str));
    sprintf(str, "DELETE /api/v2/channels/%u/data?userKey=%s HTTP/1.1\r\n", this->channelId, userKey);
    if (this->port == 80) {
        sprintf(&str[strlen(str)], "Host: %s\r\n", this->host);
    } else {
        sprintf(&str[strlen(str)], "Host: %s:%d\r\n", this->host, this->port);
    }
    sprintf(&str[strlen(str)], "Content-Length: 0\r\n");
    sprintf(&str[strlen(str)], "Content-Type: application/json\r\n\r\n");
    DBG(str);

    int ret;
    ret = this->client->print(str);
    delay(30);
    DBG(ret);DBG(" bytes sent\r\n");
    if (ret == 0) {
        ERR("send failed\r\n");
        return false;
    }

    getStatusCode();
    while (this->client->available()) {
        inChar = this->client->read();
#if AMBIENT_DEBUG
        Serial.write(inChar);
#endif
    }

    this->client->stop();

    for (int i = 0; i < AMBIENT_NUM_PARAMS; i++) {
        this->data[i].set = false;
    }
    this->cmnt.set = false;

    return true;
}

bool
Ambient::getchannel(const char * userKey, const char * devKey, unsigned int & channelId, char * writeKey, int len, WiFiClient * c, uint32_t tmout, int dev) {
    this->status = 0;
    if(NULL == c) {
        ERR("Socket Pointer is NULL, open a socket.");
        return false;
    }
    this->client = c;
    this->dev = dev;
    if (dev) {
        strcpy(this->host, AMBIENT_HOST_DEV);
        this->port = AMBIENT_PORT_DEV;
    } else {
        strcpy(this->host, AMBIENT_HOST);
        this->port = AMBIENT_PORT;
    }

    if (connect2host(tmout) == false) {
        return false;
    }

    char str[1024];
    char inChar;

    memset(str, 0, sizeof(str));
    sprintf(str, "GET /api/v2/channels/?userKey=%s&devKey=%s HTTP/1.1\r\n", userKey, devKey);
    if (this->port == 80) {
        sprintf(&str[strlen(str)], "Host: %s\r\n", this->host);
    } else {
        sprintf(&str[strlen(str)], "Host: %s:%d\r\n", this->host, this->port);
    }
    sprintf(&str[strlen(str)], "Content-Type: application/json\r\n\r\n");
    DBG(str);

    int ret;
    ret = this->client->print(str);
    delay(30);
    DBG(ret);DBG(" bytes sent\r\n");
    if (ret == 0) {
        ERR("send failed\r\n");
        return false;
    }

    if (getStatusCode() != 200) {  // ステータスコードが200でなければ
        while (this->client->available()) {
            this->client->readStringUntil('\n');
        }
        return false;
    }
    while (this->client->available()) {
        String buf = this->client->readStringUntil('\n');
        if (buf.length() == 1)
            break;
    }
    String buf = this->client->readStringUntil('\n');

    int from, to;
    from = buf.indexOf("\"ch\":\"") + strlen("\"ch\":\"");
    to = buf.indexOf("\",", from);
    channelId = buf.substring(from, to).toInt();
    from = buf.indexOf("\"writeKey\":\"") + strlen("\"writeKey\":\"");
    to = buf.indexOf("\",", from);
    buf.substring(from, to).toCharArray(writeKey, len);

    this->client->stop();

    return true;
}
