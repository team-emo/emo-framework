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

#include "SquirrelGlue.h"

using namespace std;

namespace emo {

    class Database;
    class CipherHolder {
    public:
        CipherHolder();
        CipherHolder(string text, bool isCipher);
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
        ColumnHolder(CipherHolder name, bool primaryFlag);
        ColumnHolder(string name, bool primaryFlag);
        ~ColumnHolder();
        CipherHolder getColumnName();
        bool isPrimaryColumn();
    protected:
        CipherHolder columnName;
        bool isPrimary;
    };

    class TableHolder {
    public:
        TableHolder(CipherHolder name);
        ~TableHolder();
        void addColumn(ColumnHolder column);
        CipherHolder getTableName();
        ColumnHolder* getColumnHolder(string columnName);
        ColumnHolder* getColumnHolder(int index);
        vector<ColumnHolder>* getColumns();
    protected:
        CipherHolder tableName;
        vector<ColumnHolder> columns;
    };

    class ConditionBuilder {
    public:
        ConditionBuilder(Database* db, TableHolder* tbl, bool encryptFlag);
        ~ConditionBuilder();
        bool createCondition(Sqrat::Object condArray);

        string getCondition();
        bool isValidCondition();
        bool addCondition(string colName, string eqSign, string value);
        bool addConjunction(string conjunction);
        bool addParenthesis(string parenthesis);

    protected:
        Database* database;
        TableHolder* table;
        string condition;
        bool isEncrypted;
        bool isValid;
        int leftParenthCount;
        int rightParenthCount;
    };

    class Database {
    public:
        Database();
        ~Database();

        int lastError;
        string lastErrorMessage;

        // Primary Functions
        string getPath(string name);
        string create(string name, jint mode);
        bool remove(string name);
        bool openOrCreate(string name, jint mode, bool encryptFlag);
        bool open(string name, bool encryptFlag);
        bool close();
        void analyze();
        void addTableHolder(TableHolder& table);
        TableHolder* getTableHolder(string tableName);
        bool isDuplicatedKey(string& tableName, int index, CipherHolder& value, bool encryptFlag);

        // SQL Functions
        int createTable(const string& tableName, vector<string>& columns, bool primaryFlag);
        int dropTable(const string& tableName);
        int select(vector<emo::CipherHolder>* values, const string& targetColumn, const string& tableName);
        int selectWhere(vector<emo::CipherHolder>* values, const string& targetColumn, const string& tableName, ConditionBuilder& con);
        int selectAll(vector< vector<emo::CipherHolder> >* values, const string& tableName);
        int selectAllWhere(vector< vector<emo::CipherHolder> >* values, const string& tableName, ConditionBuilder& con);
        int selectBinary(char** value, const string& contentColumn, const string& tableName, const string& nameColumn, const string& targetName);
        int selectBinaryCipher(char** value, const string& contentColumn, const string& tableName, const string& nameColumn, const string& targetName);
        int count(int* value, const string& tableName);
        int countWhere(int* value, const string& tableName,  ConditionBuilder& con);
        int insert(const string& tableName, vector<string>& values);
        int update(const string& tableName, vector< vector<string> >& columns);
        int updateWhere(const string& tableName, vector< vector<string> >& columns, ConditionBuilder& con);
        int deleteWhere(const string& tableName, ConditionBuilder& con);
        int truncate(const string& tableName);
        int vacuum(void);

        // Preference Operation
        bool   openOrCreatePreference(bool encryptFlag);
        string getPreference(string key);
        bool   setPreference(string key, string value);
        vector<emo::CipherHolder> getPreferenceKeys();
        bool deletePreference(string key);

        // Assets Acquisition
        char* getConfig();
        char* getScript(const char* name);

        // Native Closures
        int sqOpenOrCreate(bool encryptFlag, string databaseName, int mode);
        int sqOpen(bool encryptFlag, string databaseName);
        int sqClose();
        int sqRemove(string databaseName);
        int sqCreateTable(string tableName, Sqrat::Object columnNames, bool primaryFlag);
        int sqDropTable(string tableName);
        Sqrat::Object sqSelect(string targetColumn, string tableName);
        Sqrat::Object sqSelectAll(string tableName);
        Sqrat::Object sqSelectWhere(string targetColumn, string tableName, Sqrat::Object conds);
        Sqrat::Object sqSelectAllWhere(string tableName, Sqrat::Object conds);
        int sqCount(string tableName);
        int sqCountWhere(string tableName, Sqrat::Object conds);
        int sqInsert(string tableName, Sqrat::Object values);
        int sqUpdate(string tableName, Sqrat::Object dataSets);
        int sqUpdateWhere(string tableName, Sqrat::Object dataSets, Sqrat::Object conds);
        int sqTruncate(string tableName);
        int sqDeleteWhere(string tableName, Sqrat::Object conds);
        int sqVacuum();
        string sqGetPath(string databaseName);
        int    sqGetLastError();
        string sqGetLastErrorMessage();

    protected:
        sqlite3* db;
        bool isOpen;
        bool isAnalyzed;
        vector<TableHolder>* tables;

        int exec(char* sql, bool freeSqlFlag);
        int query(char* sql, sqlite3_callback callback, void* values, bool freeSqlFlag);
    };

    class Preference {
    public:
        int sqOpenOrCreatePreference(bool encryptFlag);
        string sqGetPreference(string key);
        int sqSetPreference(string key, string value);
        int sqDeletePreference(string key);
        Sqrat::Object sqGetPreferenceKeys();
    };
}

#endif
