#import "squirrel.h"

void LOGI(const char* msg);
void LOGE(const char* msg);
void LOGW(const char* msg);

void NSLOGI(NSString* msg);
void NSLOGE(NSString* msg);
void NSLOGW(NSString* msg);

void registerEmoClass(HSQUIRRELVM v, const char *cname);
void registerEmoClassFunc(HSQUIRRELVM v, const char *cname, const char *fname, SQFUNCTION func);
