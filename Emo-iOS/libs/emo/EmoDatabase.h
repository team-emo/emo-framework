#import <Foundation/Foundation.h>
#import <sqlite3.h>

@interface EmoDatabase : NSObject {
	sqlite3* db;
	BOOL isOpen;
	NSInteger lastError;
	NSString* lastErrorMessage;
}
@property (readwrite) NSInteger lastError;
@property (readwrite, copy) NSString* lastErrorMessage;

-(BOOL)open:(NSString*) name;
-(BOOL)openOrCreate:(NSString*)name mode:(NSInteger)mode;
-(BOOL)close;
-(BOOL)deleteDatabase:(NSString*)name;
-(BOOL)openOrCreatePreference;
-(BOOL)openPreference;
-(NSString*)getPreference:(NSString*)key;
-(BOOL)setPreference:(NSString*) key value:(NSString*)value;
-(NSArray*)getPreferenceKeys;
-(BOOL)deletePreference:(NSString*)key;
-(NSInteger)getLastError;
-(NSString*)getLastErrorMessage;
-(NSString*)getPath:(NSString*)name;
@end
