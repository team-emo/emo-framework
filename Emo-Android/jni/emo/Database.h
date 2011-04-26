#ifndef EMO_DATABASE_H
#define EMO_DATABASE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include <jni.h>

namespace emo {
    class Database {
    public:
        Database();
        ~Database();
        std::string getPath(std::string name);
        std::string create(std::string name, jint mode);
        int lastError;
        std::string lastErrorMessage;
        bool openOrCreate(std::string name);
        bool open(std::string name);
        int  exec(std::string sql);
        int  exec_count(std::string sql, int* count);
        int  query_vector(std::string sql, std::vector<std::string>* values);
        bool close();
        bool openOrCreatePreference();
        bool openPreference();
        std::string getPreference(std::string key, bool forceClose);
        bool setPreference(std::string key, std::string value, bool forceClose);
        std::vector<std::string> getPreferenceKeys(std::string key, bool forceClose);
        bool deletePreference(std::string key, bool forceClose);
    protected:
        sqlite3* db;
        bool isOpen;
    };
}
#endif