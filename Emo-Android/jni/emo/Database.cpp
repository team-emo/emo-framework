#include "Database.h"

#include <sstream>
#include <stdlib.h>

#include "Constants.h"
#include "Runtime.h"
#include "Engine.h"

extern emo::Engine* engine;

static int database_preference_callback(void *arg, int argc, char **argv, char **column)  {
    std::string* value = (std::string*)arg;
    if (argc > 0) *value = argv[0];
    return SQLITE_OK;
}

static int database_count_callback(void *arg, int argc, char **argv, char **column)  {
    int* count = (int*)arg;
    if (argc > 0) *count = atoi(argv[0]);
    return SQLITE_OK;
}

static int database_query_vector_callback(void *arg, int argc, char **argv, char **column)  {
    std::vector<std::string>* values = (std::vector<std::string>*)arg;

    for (int i = 0; i < argc; i++) {
        values->push_back(argv[i]);
    }

    return SQLITE_OK;
}

namespace emo {

    Database::Database() {
        this->isOpen = false;
        this->lastError = SQLITE_OK;
    }

    Database::~Database() {

    }

    std::string Database::getPath(std::string name) {
        std::string databasePath;
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "getDatabasePath", "(Ljava/lang/String;)Ljava/io/File;");
        jobject filej = env->CallObjectMethod(engine->app->activity->clazz, methodj, env->NewStringUTF(name.c_str()));
        if (filej != NULL) {
            clazz = env->GetObjectClass(filej);
            methodj = env->GetMethodID(clazz, "getPath", "()Ljava/lang/String;");
            jstring jstr = (jstring)env->CallObjectMethod(filej, methodj);
            if (jstr != NULL) {
                const char* str = env->GetStringUTFChars(jstr, NULL);
                databasePath = str;
                env->ReleaseStringUTFChars(jstr, str);
            }
        }
        vm->DetachCurrentThread();

        return databasePath;
    }

    std::string Database::create(std::string name, jint mode) {
        std::string filename;
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "openOrCreateDatabase", "(Ljava/lang/String;ILandroid/database/sqlite/SQLiteDatabase/CursorFactory;)Landroid/database/sqlite/SQLiteDatabase;");
        jobject jdb = env->CallObjectMethod(engine->app->activity->clazz, methodj,  env->NewStringUTF(name.c_str()), mode, NULL);
        if (jdb != NULL) {
            clazz = env->GetObjectClass(jdb);
            methodj = env->GetMethodID(clazz, "getPath", "()Ljava/lang/String;");
            jstring jstr = (jstring)env->CallObjectMethod(jdb, methodj);
            if (jstr != NULL) {
                const char* str = env->GetStringUTFChars(jstr, NULL);
                filename = str;
                env->ReleaseStringUTFChars(jstr, str);
            }
            methodj = env->GetMethodID(clazz, "close", "()V");
            env->CallVoidMethod(jdb, methodj);
        }
        vm->DetachCurrentThread();

        return filename;
    }

    bool Database::openOrCreate(std::string name) {
        if (this->isOpen) return false;
        
        std::string path = this->create(name, FILE_MODE_PRIVATE);
        
        int rcode = sqlite3_open(path.c_str(), &this->db);
        
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage =  sqlite3_errmsg(this->db);
        }
        
        this->isOpen = true;
        return true;
    }

    bool Database::open(std::string name) {
        if (this->isOpen) return false;

        std::string path = this->getPath(name);

        int rcode = sqlite3_open(path.c_str(), &this->db);
        
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage =  sqlite3_errmsg(this->db);
        }
        
        this->isOpen = true;
        return true;
    }

    bool Database::close() {
        if (!this->isOpen) return false;

        sqlite3_close(this->db);

        this->isOpen = false;
        return true;
    }

    int Database::exec(std::string sql) {
        char* error;
        int rcode = sqlite3_exec(this->db, sql.c_str(), NULL, 0, &error);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage = error;
            sqlite3_free(error);
        }
        return rcode;
    }

    int Database::exec_count(std::string sql, int* count) {
        char* error;
        int rcode = sqlite3_exec(this->db, sql.c_str(), database_count_callback, count, &error);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage = error;
            sqlite3_free(error);
        }
        return rcode;
    }

    int Database::query_vector(std::string sql, std::vector<std::string>* values) {
        char* error;
        int rcode = sqlite3_exec(this->db, sql.c_str(), database_query_vector_callback, values, &error);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage = error;
            sqlite3_free(error);
        }
        return rcode;
    }

    bool Database::openOrCreatePreference() {
        return this->openOrCreate(DEFAULT_DATABASE_NAME);
    }

    bool Database::openPreference() {
        return this->open(DEFAULT_DATABASE_NAME);
    }

    std::string Database::getPreference(std::string key, bool forceClose) {
        if (!this->isOpen) return false;
        std::string value;

        char* sql = sqlite3_mprintf("SELECT VALUE FROM %q WHERE KEY=%Q", PREFERENCE_TABLE_NAME, key.c_str());

        char* error;
        int rcode = sqlite3_exec(this->db, sql, database_preference_callback, &value, &error);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage = error;
            sqlite3_free(error);
        }

        if (forceClose) {
            this->close();
        }
        return value;
    }

    bool Database::setPreference(std::string key, std::string value, bool forceClose) {
        if (!this->isOpen) return false;

        int count;
        char *csql = sqlite3_mprintf("SELECT COUNT(*) FROM %q WHERE KEY=%Q", PREFERENCE_TABLE_NAME, key.c_str());
        exec_count(csql, &count);
        sqlite3_free(csql);

        int rcode = SQLITE_OK;
        if (count == 0) {
            char* sql = sqlite3_mprintf("INSERT INTO %q(KEY,VALUE) VALUES(%Q,%Q)", PREFERENCE_TABLE_NAME, key.c_str(), value.c_str());
            rcode = this->exec(sql);
            sqlite3_free(sql);
        } else {
            char* sql = sqlite3_mprintf("UPDATE %q SET VALUE=%Q WHERE KEY=%Q", PREFERENCE_TABLE_NAME, value.c_str(), key.c_str());
            rcode = this->exec(sql);
            sqlite3_free(sql);
        }

        if (forceClose) {
            this->close();
        }

        return rcode == SQLITE_OK;
    }

    std::vector<std::string> Database::getPreferenceKeys(std::string key, bool forceClose) {
        std::vector<std::string> keys;
        if (!this->isOpen) return keys;

        char *sql = sqlite3_mprintf("SELECT KEY FROM %q", PREFERENCE_TABLE_NAME);
        query_vector(sql, &keys);
        sqlite3_free(sql);

        if (forceClose) {
            this->close();
        }
        return keys;
    }

    bool Database::deletePreference(std::string key, bool forceClose) {
        if (!this->isOpen) return false;

        char *sql = sqlite3_mprintf("DELETE FROM %q WHERE KEY=%Q", PREFERENCE_TABLE_NAME, key.c_str());
        int rcode = this->exec(sql);
        sqlite3_free(sql);

        if (forceClose) {
            this->close();
        }
        return rcode == SQLITE_OK;
    }

}
