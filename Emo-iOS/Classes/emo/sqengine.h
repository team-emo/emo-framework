void LOGI(const char* msg);
void LOGE(const char* msg);
void LOGW(const char* msg);

void NSLOGI(NSString* msg);
void NSLOGE(NSString* msg);
void NSLOGW(NSString* msg);

@interface EmoEngine : NSObject {
	int lastError;
}
@property int lastError;

- (BOOL)loadScriptFromResource:(NSString *)fname;
@end

