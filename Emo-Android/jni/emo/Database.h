#ifndef EMO_DATABASE_H
#define EMO_DATABASE_H

#include <string>
#include <vector>
#include <jni.h>
#include "sqlite3.h"
#include "squirrel.h"

void initDatabaseFunctions();

namespace emo {
    class Database {
    public:
        Database();
        ~Database();
        std::string getPath(std::string name);
        std::string create(std::string name, jint mode);
        int lastError;
        std::string lastErrorMessage;
        bool openOrCreate(std::string name, jint mode);
        bool open(std::string name);
        bool close();
        bool deleteDatabase(std::string name);
        bool openOrCreatePreference();
        bool openPreference();
        std::string getPreference(std::string key);
        bool setPreference(std::string key, std::string value);
        std::vector<std::string> getPreferenceKeys();
        bool deletePreference(std::string key);
    protected:
        sqlite3* db;
        bool isOpen;
        int  exec(std::string sql);
        int  exec_count(std::string sql, int* count);
        int  query_vector(std::string sql, std::vector<std::string>* values);
    };
}

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
#endif