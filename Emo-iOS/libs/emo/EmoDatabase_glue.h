#import "squirrel.h"

void initDatabaseFunctions();

SQInteger emoDatabaseOpenOrCreate(HSQUIRRELVM v);
SQInteger emoDatabaseOpen(HSQUIRRELVM v);
SQInteger emoDatabaseClose(HSQUIRRELVM v);
SQInteger emoDatabaseGetPath(HSQUIRRELVM v);
SQInteger emoDatabaseGetLastError(HSQUIRRELVM v);
SQInteger emoDatabaseGetLastErrorMessage(HSQUIRRELVM v);
SQInteger emoDatabaseOpenPreference(HSQUIRRELVM v);
SQInteger emoDatabaseOpenOrCreatePreference(HSQUIRRELVM v);
SQInteger emoDatabaseGetPreference(HSQUIRRELVM v);
SQInteger emoDatabaseSetPreference(HSQUIRRELVM v);
SQInteger emoDatabaseDeletePreference(HSQUIRRELVM v);
SQInteger emoDatabaseGetPreferenceKeys(HSQUIRRELVM v);
SQInteger emoDatabaseDeleteDatabase(HSQUIRRELVM v);
