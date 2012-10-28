// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#ifndef EMO_DATABASE_H
#define EMO_DATABASE_H

#include <string>
#include <vector>
#include <jni.h>
#include "sqlite3.h"
#include "squirrel.h"

using namespace std;

void initDatabaseFunctions();

namespace emo {

    class CipherHolder {
    public:
        CipherHolder();
        ~CipherHolder();
        string getPlainText();
        void setPlainText(string plainText);
        string getCipher();
        void setCipher(string cipher);
        bool hasCipher();
        bool compareWithCipher(string cipher);
        bool compareWithHolder(CipherHolder holder);
    protected:
        string plainText;
        string cipher;
    };

    class ColumnHolder {
    public:
        ColumnHolder(CipherHolder name);
        ColumnHolder(string name);
        CipherHolder getColumnName();
        ~ColumnHolder();
    protected:
        CipherHolder columnName;
    };

    class TableHolder {
    public:
        TableHolder(CipherHolder name);
        ~TableHolder();
        void addColumn(ColumnHolder column);
        CipherHolder getTableName();
        ColumnHolder* getColumnHolder(string columnName);
    protected:
        CipherHolder tableName;
        vector<ColumnHolder> columns;
    };

    class Database {
    public:
        Database();
        ~Database();

        int lastError;
        string lastErrorMessage;

        string getPath(string name);
        string create(string name, jint mode);

        bool open(string name);
        bool openOrCreate(string name, jint mode);
        bool openDatabase();
        bool close();
        bool deleteDatabase(string name);
        void analyze();
        TableHolder* getTableHolder(string tableName);

        bool openOrCreatePreference();
        string getPreference(string key);
        bool setPreference(string key, string value);
        vector<emo::CipherHolder> getPreferenceKeys();
        bool deletePreference(string key);

        char* getConfig();
        char* getScript(const char* name);

    protected:
        sqlite3* db;
        bool isOpen;
        bool isAnalyzed;
        vector<TableHolder>* tables;

        int exec(char* sql, bool freeSqlFlag);
        int query(char* sql, sqlite3_callback callback, void* values, bool freeSqlFlag);
        int selectStringCipher(emo::CipherHolder* value, const string& contentColumn, const string& tableName, const string& keyColumn, const string& keyValue);
        int selectStringCiphers(vector<emo::CipherHolder>* value, const string& targetColumn, const string& tableName);
        int selectBinaryContent(char** value, const string& contentColumn, const string& tableName, const string& nameColumn, const string& targetName);
        int selectBinaryCipherContent(char** value, const string& contentColumn, const string& tableName, const string& nameColumn, const string& targetName);
        int insertStrings(const string& tableName, vector<string>& values);
        int updateString(const string& tableName, const string& targetColumn, const string& value, const string& keyColumn, const string& keyValue);
        int deleteRecords(const string& tableName, const string& keyColumn, const string& keyValue);

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
