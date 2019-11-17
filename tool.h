#include <json/json.h>
Json::Value stream2json(unsigned char stream[]);
unsigned char * json2stream(Json::Value value);