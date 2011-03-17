void LOGI(const char* msg);
void LOGE(const char* msg);
void LOGW(const char* msg);

void NSLOGI(NSString* msg);
void NSLOGE(NSString* msg);
void NSLOGW(NSString* msg);

@interface EmoEngine : NSObject {
	int lastError;
	BOOL enableSQOnDrawFrame;
}
@property int lastError;
@property BOOL enableSQOnDrawFrame;

- (BOOL)loadScriptFromResource:(NSString *)fname;
- (BOOL)startEngine
- (BOOL)stopEngine
@end

