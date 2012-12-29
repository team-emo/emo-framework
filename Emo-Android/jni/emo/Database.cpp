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
#include "Database.h"

#include <sstream>
#include <stdlib.h>

#include "Constants.h"
#include "Runtime.h"
#include "Engine.h"
#include "VmFunc.h"
#include "Util.h"

using namespace std;
extern emo::Engine* engine;

void initDatabaseFunctions() {
    registerClass(engine->sqvm, EMO_DATABASE_CLASS);
    registerClass(engine->sqvm, EMO_PREFERENCE_CLASS);

    registerClassFunc(engine->sqvm, EMO_DATABASE_CLASS, "openOrCreate", emoDatabaseOpenOrCreate);
    registerClassFunc(engine->sqvm, EMO_DATABASE_CLASS, "open",         emoDatabaseOpen);
    registerClassFunc(engine->sqvm, EMO_DATABASE_CLASS, "close",        emoDatabaseClose);
    registerClassFunc(engine->sqvm, EMO_DATABASE_CLASS, "getPath",      emoDatabaseGetPath);
    registerClassFunc(engine->sqvm, EMO_DATABASE_CLASS, "getLastError", emoDatabaseGetLastError);
    registerClassFunc(engine->sqvm, EMO_DATABASE_CLASS, "getLastErrorMessage", emoDatabaseGetLastErrorMessage);
    registerClassFunc(engine->sqvm, EMO_DATABASE_CLASS, "deleteDatabase", emoDatabaseDeleteDatabase);

    registerClassFunc(engine->sqvm, EMO_PREFERENCE_CLASS, "open",         emoDatabaseOpenPreference);
    registerClassFunc(engine->sqvm, EMO_PREFERENCE_CLASS, "openOrCreate", emoDatabaseOpenOrCreatePreference);
    registerClassFunc(engine->sqvm, EMO_PREFERENCE_CLASS, "get",          emoDatabaseGetPreference);
    registerClassFunc(engine->sqvm, EMO_PREFERENCE_CLASS, "set",          emoDatabaseSetPreference);
    registerClassFunc(engine->sqvm, EMO_PREFERENCE_CLASS, "del",          emoDatabaseDeletePreference);
    registerClassFunc(engine->sqvm, EMO_PREFERENCE_CLASS, "keys",         emoDatabaseGetPreferenceKeys);
    registerClassFunc(engine->sqvm, EMO_PREFERENCE_CLASS, "close",        emoDatabaseClose);
}

static int database_callback_count(void *arg, int argc, char **argv, char **column)  {
    int* count = (int*)arg;
    if (argc > 0) *count = atoi(argv[0]);

    return SQLITE_OK;
}

static int database_callback_string_cipher(void *arg, int argc, char **argv, char **column)  {
    vector<emo::CipherHolder>* values = (vector<emo::CipherHolder>*)arg;

    for (int i = 0; i < argc; i++) {
        emo::CipherHolder holder = emo::CipherHolder();
        holder.setCipher( string(argv[i]) );
        values->push_back(holder);
    }

    return SQLITE_OK;
}

static int database_callback_multiple_string_cipher(void *arg, int argc, char **argv, char **column)  {
    vector<vector< emo::CipherHolder> >* values = (vector< vector<emo::CipherHolder> >*)arg;

    vector<emo::CipherHolder> cols;
    for (int i = 0; i < argc; i++) {
        if(argv[i] == NULL) continue;
        emo::CipherHolder holder = emo::CipherHolder();
        holder.setCipher( string(argv[i]) );
        cols.push_back( holder );
    }
    values->push_back(cols);

    return SQLITE_OK;
}

static int database_callback_binary(void *arg, int argc, char **argv, char **column)  {
    vector<unsigned char*>* values = (vector<unsigned char*>*)arg;

    if (argc > 0){
        int len = strlen(argv[0]);
        unsigned char* binary =  (unsigned char*) calloc( len + 1, sizeof(char));
        memcpy(binary, argv[0], len + 1);
        values->push_back( binary );
    }

    return SQLITE_OK;
}

static int database_callback_binary_cipher(void *arg, int argc, char **argv, char **column)  {
    vector<unsigned char*>* values = (vector<unsigned char*>*)arg;

    if(argc > 0){
        int plainSize = 0;
        unsigned char * decryptedBytes = decrypt( (unsigned char *)AES_PRIVATE_KEY, (unsigned char *)argv[0], atoi(argv[1]), &plainSize);
        values->push_back(decryptedBytes);
    }

    return SQLITE_OK;
}

namespace emo {

    CipherHolder::CipherHolder() {
        this->plainText  = string();
        this->cipher = string();
    }

    CipherHolder::~CipherHolder() {

    }

    CipherHolder::CipherHolder(string text, bool cipherFlag) {
        if(cipherFlag == true){
            this->setCipher(text);
        }else{
            this->setPlainText(text);
        }
    }


    string CipherHolder::getCipher(){
        return this->cipher;;
    }

    void CipherHolder::setCipher(string cipher){
        this->cipher = cipher;
        this->plainText  = decryptString(cipher);

        // if the string cannot be decrypted, save it as a plain text
        if(this->plainText.empty() == true){
            this->plainText = cipher;
        }
    }

    string CipherHolder::getPlainText(){
        return this->plainText;;
    }

    void CipherHolder::setPlainText(string plainText){
        this->plainText = plainText;
        this->cipher    = encryptString(plainText);
    }

    bool CipherHolder::hasCipher() {
        if(cipher.size() != 0){
            return true;
        }
        return false;
    }

    bool CipherHolder::compareWithCipher(string cipher) {
        if(plainText == decryptString(cipher)){
            return true;
        }
        return false;
    }

    bool CipherHolder::compareWithHolder(CipherHolder holder) {
        if(plainText == holder.getPlainText()){
            return true;
        }
        return false;
    }

    ColumnHolder::ColumnHolder(CipherHolder name){
        this->columnName = name;
    }

    ColumnHolder::ColumnHolder(string name){
        this->columnName = emo::CipherHolder();
        this->columnName.setPlainText(name);
    }

    emo::CipherHolder ColumnHolder::getColumnName(){
        return this->columnName;
    }

    ColumnHolder::~ColumnHolder(){

    }

    TableHolder::TableHolder(CipherHolder name){
        this->tableName = name;
    }

    TableHolder::~TableHolder(){

    }

    void TableHolder::addColumn(ColumnHolder column){
        this->columns.push_back(column);
    }

    ColumnHolder* TableHolder::getColumnHolder(string columnName){
        vector<emo::ColumnHolder>::iterator itCols;
        for(itCols = this->columns.begin(); itCols != this->columns.end(); itCols++){
            if(itCols->getColumnName().getPlainText() == columnName){
                return itCols;
            }
        }
        return NULL;
    }

    CipherHolder TableHolder::getTableName(){
        return this->tableName;
    }

    Database::Database() {
        this->isOpen = false;
        this->isAnalyzed = false;
        this->lastError = SQLITE_OK;
        this->tables = new vector<TableHolder>();
    }

    Database::~Database() {
        delete tables;
    }

    string Database::getPath(string name) {
        string databasePath;
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

    string Database::create(string name, jint mode) {
        string filename;
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "openOrCreateDatabase", "(Ljava/lang/String;ILandroid/database/sqlite/SQLiteDatabase$CursorFactory;)Landroid/database/sqlite/SQLiteDatabase;");
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

    bool Database::deleteDatabase(string name) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "deleteDatabase", "(Ljava/lang/String;)Z");
        jboolean deleted = env->CallBooleanMethod(engine->app->activity->clazz, methodj, env->NewStringUTF(name.c_str()));

        vm->DetachCurrentThread();

        return deleted;
    }

    bool Database::open(string name) {
        if (this->isOpen) return false;

        string path = this->getPath(name);

        int rcode = sqlite3_open(path.c_str(), &this->db);
        
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage =  sqlite3_errmsg(this->db);
        }
        
        this->isOpen = true;
        if(!this->isAnalyzed) this->analyze();

        return true;
    }

    bool Database::openOrCreate(string name, jint mode) {
        if (this->isOpen) return false;

        string path = this->create(name, mode);

        int rcode = sqlite3_open(path.c_str(), &this->db);
        
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage =  sqlite3_errmsg(this->db);
        }
        
        this->isOpen = true;
        // analyze DB to get names of all the tables and columns
        if(!this->isAnalyzed) this->analyze();

        return true;
    }

    bool Database::openDatabase() {
        return this->open(DEFAULT_DATABASE_NAME);
    }

    bool Database::close() {
        if (!this->isOpen) return false;

        sqlite3_close(this->db);

        this->isOpen = false;
        return true;
    }

    void Database::analyze(){
        if(this->isAnalyzed == true) return;

        // retrieve all tables' name
        vector<emo::CipherHolder> tableNames;
        char sqlTables [64] = "SELECT NAME FROM sqlite_master WHERE TYPE='table'";
        this->query(sqlTables, database_callback_string_cipher, &tableNames, false);

        vector<emo::CipherHolder>::iterator itTables;
        for(itTables = tableNames.begin(); itTables != tableNames.end(); itTables++){
            emo::TableHolder table = emo::TableHolder(*itTables);
            // retrieve table's information
            vector< vector<emo::CipherHolder> > tableInfos;
            char *sqlColumns = sqlite3_mprintf("PRAGMA TABLE_INFO('%q')", itTables->getCipher().c_str());
            this->query(sqlColumns, database_callback_multiple_string_cipher, &tableInfos, true);

            vector<vector<emo::CipherHolder> >::iterator itCols;
            for(itCols = tableInfos.begin(); itCols != tableInfos.end(); itCols++){
                emo::ColumnHolder column = emo::ColumnHolder(itCols->at(1));
                table.addColumn(column);
            }
            this->tables->push_back(table);
        }
        this->isAnalyzed = true;
    }

    TableHolder* Database::getTableHolder(string tableName){
        // find table
        vector<emo::TableHolder>::iterator itTables;
        for(itTables = this->tables->begin(); itTables != this->tables->end(); itTables++){
            if(itTables->getTableName().getPlainText() == tableName){
                return itTables;
            }
        }
        return NULL;
    }

    int Database::exec(char* sql, bool freeSqlFlag) {
        char* error;
        int rcode = sqlite3_exec(this->db, sql, NULL, 0, &error);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage = error;
            sqlite3_free(error);
        }
        if( freeSqlFlag) sqlite3_free(sql);

        return rcode;
    }

    int Database::query(char* sql, sqlite3_callback callback, void * values, bool freeSqlFlag) {
        char* error;
        int rcode = sqlite3_exec(this->db, sql, callback, values, &error);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage = error;
            sqlite3_free(error);
        }
        if( freeSqlFlag) sqlite3_free(sql);

        return rcode;
    }

    int Database::createTable(const string& tableName, vector<string>& columns, bool primaryFlag){
        string stringValues = string("");

        bool firstFlag = true;
        vector<string>::iterator itColumns;
        for(itColumns = columns.begin(); itColumns != columns.end(); itColumns++){
            if(firstFlag == true){
                firstFlag = false;
                stringValues += "\"" + *itColumns + "\" TEXT";
                if(primaryFlag) stringValues += " PRIMARY KEY";
            }else{
                stringValues += ", \"" + *itColumns + "\" TEXT";
            }
        }
        string sqlString = "CREATE TABLE IF NOT EXISTS '" + tableName + "' (" + stringValues + ")";

        return this->exec((char*)sqlString.c_str(), false);
    }

    int Database::dropTable(const string& tableName){
        char *sql = sqlite3_mprintf( "DROP TABLE %Q", tableName.c_str() );
        return this->exec(sql, true);
    }

    int Database::select(vector<emo::CipherHolder>* values, const string& targetColumn, const string& tableName){
        char *sql = sqlite3_mprintf(
                "SELECT \"%q\" FROM %Q",
                targetColumn.c_str(), tableName.c_str() );

        int rcode = this->query(sql, database_callback_string_cipher, values, true);
        return rcode;
    }

    int Database::selectWhere(vector<emo::CipherHolder>* values, const string& targetColumn, const string& tableName, const string& keyColumn, const string& keyValue){
        char *sql = sqlite3_mprintf(
                "SELECT \"%q\" FROM %Q WHERE \"%q\"=\"%q\"",
                targetColumn.c_str(), tableName.c_str(), keyColumn.c_str(), keyValue.c_str() );

        int rcode = this->query(sql, database_callback_string_cipher, values, true);
        return rcode;
    }

    int Database::selectAll(vector< vector<emo::CipherHolder> >* values, const string& tableName){
        char *sql = sqlite3_mprintf( "SELECT * FROM %Q", tableName.c_str() );

        int rcode = this->query(sql, database_callback_multiple_string_cipher, values, true);
        return rcode;
    }

    int Database::selectAllWhere(vector< vector<emo::CipherHolder> >* values, const string& tableName, const string& keyColumn, const string& keyValue){
        char *sql = sqlite3_mprintf(
                "SELECT * FROM %Q WHERE \"%q\"=\"%q\"",
                tableName.c_str(), keyColumn.c_str(), keyValue.c_str() );

        int rcode = this->query(sql, database_callback_multiple_string_cipher, values, true);
        return rcode;
    }

    int Database::selectBinary(char** value, const string& contentColumn, const string& tableName, const string& nameColumn, const string& targetName){
        vector<unsigned char*> binaryContents;

        char *sql = sqlite3_mprintf(
                "SELECT \"%q\" FROM %Q WHERE \"%q\"=\"%q\"",
                contentColumn.c_str(), tableName.c_str(), nameColumn.c_str(), targetName.c_str() );

        int rcode = this->query(sql, database_callback_binary, &binaryContents, true);
        if( rcode == SQLITE_OK ){
            *value = (char*)binaryContents.at(0);
        }
        return rcode;
    }

    int Database::selectBinaryCipher(char** value, const string& contentColumn, const string& tableName, const string& nameColumn, const string& targetName){
        vector<unsigned char*> binaryContents;

        char *sql = sqlite3_mprintf(
                "SELECT \"%q\", length(\"%q\") FROM %Q WHERE \"%q\"=\"%q\"",
                contentColumn.c_str(), contentColumn.c_str(), tableName.c_str(), nameColumn.c_str(), targetName.c_str() );

        int rcode = this->query(sql, database_callback_binary_cipher, &binaryContents, true);
        if( rcode == SQLITE_OK ){
            *value = (char*)binaryContents.at(0);
        }
        return rcode;
    }

    int Database::count(int* value, const string& tableName){
        char *sql = sqlite3_mprintf( "SELECT COUNT(*) FROM %Q", tableName.c_str() );

        int rcode = this->query(sql, database_callback_count, value, true);
        return rcode;
    }

    int Database::countWhere(int* value, const string& tableName, const string& keyColumn, const string& keyValue){
        char *sql = sqlite3_mprintf(
                "SELECT COUNT(*) FROM %Q WHERE \"%q\"=\"%q\"",
                tableName.c_str(), keyColumn.c_str(), keyValue.c_str() );

        int rcode = this->query(sql, database_callback_count, value, true);
        return rcode;
    }

    int Database::insert(const string& tableName, vector<string>& values){
        string stringValues = string("");

        bool firstFlag = true;
        vector<string>::iterator itValues;
        for(itValues = values.begin(); itValues != values.end(); itValues++){
            if(firstFlag == true){
                firstFlag = false;
            }else{
                stringValues += ", ";
            }
            stringValues += "\"" + *itValues + "\"";
        }
        string sqlString = "INSERT INTO '" + tableName + "' VALUES(" + stringValues + ")";

        return this->exec((char*)sqlString.c_str(), false);
    }

    int Database::update(const string& tableName, vector<string>& columns, vector<string>& values){
        string stringValues = string("");

        bool firstFlag = true;
        vector<string>::iterator itColumns;
        vector<string>::iterator itValues;
        for(itColumns = columns.begin(), itValues = values.begin(); itColumns != columns.end(); itColumns++, itValues++){
            if(firstFlag == true){
                firstFlag = false;
            }else{
                stringValues += ", ";
            }
            stringValues += "\"" + *itColumns + "\"" + " = " + "\"" + *itValues + "\"";
        }
        string sqlString = "UPDATE '" + tableName + "' SET " + stringValues;

        return this->exec((char*)sqlString.c_str(), false);
    }

    int Database::updateWhere(const string& tableName, vector<string>& columns, vector<string>& values, const string& keyColumn, const string& keyValue){
        string stringValues = string("");

        bool firstFlag = true;
        vector<string>::iterator itColumns;
        vector<string>::iterator itValues;
        for(itColumns = columns.begin(), itValues = values.begin(); itColumns != columns.end(); itColumns++, itValues++){
            if(firstFlag == true){
                firstFlag = false;
            }else{
                stringValues += ", ";
            }
            stringValues += "\"" + *itColumns + "\"" + " = " + "\"" + *itValues + "\"";
        }
        string sqlString = "UPDATE '" + tableName + "' SET " + stringValues + " WHERE \"" + keyColumn + "\" = \"" + keyValue + "\"";

        return this->exec((char*)sqlString.c_str(), false);
    }

    int Database::deleteWhere(const string& tableName, const string& keyColumn, const string& keyValue){
        char *sql = sqlite3_mprintf("DELETE FROM %Q WHERE \"%q\"=\"%q\"",
                tableName.c_str(), keyColumn.c_str(), keyValue.c_str());
        return this->exec(sql, true);
    }

    int Database::truncate(const string& tableName){
        char *sql = sqlite3_mprintf("DELETE FROM %Q", tableName.c_str());
        return this->exec(sql, true);
    }

    int Database::vacuum(){
        char sql [8] = "VACUUM";
        return this->exec(sql, false);
    }

    bool Database::openOrCreatePreference() {
        bool result = this->openOrCreate(DEFAULT_DATABASE_NAME, FILE_MODE_PRIVATE);
        if (!result) return false;

        if(this->getTableHolder(PREFERENCE_TABLE_NAME) != NULL){
            return true;
        }
        emo::CipherHolder tableName   = emo::CipherHolder();
        emo::CipherHolder keyHolder   = emo::CipherHolder();
        emo::CipherHolder valueHolder = emo::CipherHolder();

        tableName.setPlainText(PREFERENCE_TABLE_NAME);
        keyHolder.setPlainText("KEY");
        valueHolder.setPlainText("VALUE");

        vector<string> columnNames;
#ifndef PREFERENCE_WITHOUT_ENCRYPTION
        columnNames.push_back(keyHolder.getCipher());
        columnNames.push_back(valueHolder.getCipher());
        int rcode = this->createTable(tableName.getCipher(), columnNames, true);
#else
        columnNames.push_back(keyHolder.getPlainText());
        columnNames.push_back(valueHolder.getPlainText());
        int rcode = this->createTable(tableName.getPlainText(), columnNames, true);
#endif

        if(rcode == SQLITE_OK){
            emo::TableHolder table = emo::TableHolder(tableName);
            table.addColumn(emo::ColumnHolder(keyHolder));
            table.addColumn(emo::ColumnHolder(valueHolder));
            this->tables->push_back(table);
        }
        return rcode == SQLITE_OK;
    }

    string Database::getPreference(string key) {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference();
            forceClose = true;
        }
        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);
        ColumnHolder* keyColumn       = preferenceTable->getColumnHolder("KEY");
        ColumnHolder* valueColumn     = preferenceTable->getColumnHolder("VALUE");

        vector<emo::CipherHolder> keyList = getPreferenceKeys();
        CipherHolder keyHolder;
        if( findCipherHolder(&keyHolder, keyList, key) ){
            //LOGE("Database::getPreference : key specified does not exist");
            return string();
        }

        vector<emo::CipherHolder> values;
#ifndef PREFERENCE_WITHOUT_ENCRYPTION
        this->selectWhere(
                &values,
                valueColumn->getColumnName().getCipher(),
                preferenceTable->getTableName().getCipher(),
                keyColumn->getColumnName().getCipher(),
                keyHolder.getCipher() );
#else
        this->selectWhere(
                &values,
                valueColumn->getColumnName().getPlainText(),
                preferenceTable->getTableName().getPlainText(),
                keyColumn->getColumnName().getPlainText(),
                keyHolder.getPlainText() );
#endif
        if (forceClose) {
            this->close();
        }
        return values.at(0).getPlainText();
    }

    bool Database::setPreference(string key, string value) {
        if(key.empty()) return false;

        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference();
            forceClose = true;
        }

        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);
        ColumnHolder* keyColumn       = preferenceTable->getColumnHolder("KEY");
        ColumnHolder* valueColumn     = preferenceTable->getColumnHolder("VALUE");

        // encrypt before storing to database
        string encryptedValue = encryptString(value);

        vector<emo::CipherHolder> keyList = getPreferenceKeys();
        CipherHolder keyHolder;

        int rcode = SQLITE_OK;
        if(!findCipherHolder(&keyHolder, keyList, key)){
            vector<string> values;
#ifndef PREFERENCE_WITHOUT_ENCRYPTION
            string encryptedKey = encryptString(key);

            values.push_back(encryptedKey);
            values.push_back(encryptedValue);
            rcode = this->insert(preferenceTable->getTableName().getCipher(), values);
#else
            values.push_back(key);
            values.push_back(value);
            rcode = this->insert(preferenceTable->getTableName().getPlainText(), values);
#endif
        }else{
            vector<string> columns;
            vector<string> values;
#ifndef PREFERENCE_WITHOUT_ENCRYPTION
            columns.push_back(valueColumn->getColumnName().getCipher());
            values.push_back(encryptedValue);

            rcode = this->updateWhere(preferenceTable->getTableName().getCipher(),
                    columns, values, keyColumn->getColumnName().getCipher(), keyHolder.getCipher());
#else
            columns.push_back(valueColumn->getColumnName().getPlainText());
            values.push_back(value);

            rcode = this->updateWhere(preferenceTable->getTableName().getPlainText(),
                    columns, values, keyColumn->getColumnName().getPlainText(), key);
#endif
        }

        if (forceClose) {
            this->close();
        }
        return rcode == SQLITE_OK;
    }

    vector<emo::CipherHolder> Database::getPreferenceKeys() {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference();
            forceClose = true;
        }

        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);
        ColumnHolder* keyColumn       = preferenceTable->getColumnHolder("KEY");

        vector<emo::CipherHolder> keys;
#ifndef PREFERENCE_WITHOUT_ENCRYPTION
        this->select(&keys,
                keyColumn->getColumnName().getCipher(),
                preferenceTable->getTableName().getCipher() );
#else
        this->select(&keys,
                keyColumn->getColumnName().getPlainText(),
                preferenceTable->getTableName().getPlainText() );
#endif

        if (forceClose) {
            this->close();
        }
        return keys;
    }

    bool Database::deletePreference(string key) {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference();
            forceClose = true;
        }

        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);
        ColumnHolder* keyColumn       = preferenceTable->getColumnHolder("KEY");

        vector<emo::CipherHolder> keyList = getPreferenceKeys();
        CipherHolder keyHolder;
        if( findCipherHolder(&keyHolder, keyList, key) ){
            //LOGE("Database::deletePreference : key specified does not exist");
            return false;
        }
#ifndef PREFERENCE_WITHOUT_ENCRYPTION
        int rcode = this->deleteWhere(
                preferenceTable->getTableName().getCipher(),
                keyColumn->getColumnName().getCipher(),
                keyHolder.getCipher());
#else
        int rcode = this->deleteWhere(
                preferenceTable->getTableName().getPlainText(),
                keyColumn->getColumnName().getPlainText(),
                key);
#endif

        if (forceClose) {
            this->close();
        }
        return rcode == SQLITE_OK;
    }

    char* Database::getConfig() {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openDatabase();
            forceClose = true;
        }

        TableHolder* configTable = this->getTableHolder("CONFIG");
        if(configTable == NULL) return NULL;

        ColumnHolder* contentColumn = configTable->getColumnHolder("CONTENT");
        emo::CipherHolder columnName = contentColumn->getColumnName();

        // retrieve config content
        vector<unsigned char*> configContents;
        char *sqlConfig = sqlite3_mprintf(
                "SELECT \"%q\", length(\"%q\") FROM %Q",
                columnName.getCipher().c_str(),
                columnName.getCipher().c_str(),
                configTable->getTableName().getCipher().c_str());

        this->query(sqlConfig, database_callback_binary_cipher, &configContents, true);

        if (forceClose) {
            this->close();
        }
        return (char*)configContents.at(0);
    }

    char* Database::getScript(const char* name) {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openDatabase();
            forceClose = true;
        }

        char* script = NULL;
        TableHolder* scriptTable = this->getTableHolder("SCRIPTS");
        ColumnHolder* nameColumn = scriptTable->getColumnHolder("NAME");
        ColumnHolder* contentColumn = scriptTable->getColumnHolder("CONTENT");

        emo::CipherHolder nameColumnName = nameColumn->getColumnName();
        emo::CipherHolder contentColumnName = contentColumn->getColumnName();

        if(engine->config->scriptsInfo.encrypted == true){
            // retrieve all "NAME" values from "SCRIPT" table
            vector<emo::CipherHolder> scriptNames;
            this->select(
                    &scriptNames,
                    nameColumnName.getCipher(),
                    scriptTable->getTableName().getCipher() );

            CipherHolder targetScriptName;
            findCipherHolder(&targetScriptName, scriptNames, string(name));

            if(engine->config->scriptsInfo.contentEncrypted == true){
                // retrieve script content
                this->selectBinaryCipher(
                        &script,
                        contentColumnName.getCipher(),
                        scriptTable->getTableName().getCipher(),
                        nameColumnName.getCipher(),
                        targetScriptName.getCipher() );
            }else{
                this->selectBinary(
                        &script,
                        contentColumnName.getCipher(),
                        scriptTable->getTableName().getCipher(),
                        nameColumnName.getCipher(),
                        targetScriptName.getCipher() );
            }

        }else{
            this->selectBinary(
                    &script,
                    contentColumnName.getPlainText(),
                    scriptTable->getTableName().getPlainText(),
                    nameColumnName.getPlainText(),
                    string(name) );
        }

        if (forceClose) {
            this->close();
        }
        return script;
    }

}

/*
 * open database with given name
 * if database is not found, creates new database with given name
 * file mode is one of FILE_MODE_PRIVATE, FILE_MODE_WORLD_READABLE
 * or FILE_MODE_WORLD_WRITABLE
 *
 * @param database name
 * @param file mode
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoDatabaseOpenOrCreate(HSQUIRRELVM v) {
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    SQInteger mode = FILE_MODE_PRIVATE;
    if (nargs >= 3 && sq_gettype(v, 3) == OT_INTEGER) {
        sq_getinteger(v, 3, &mode);
    }

    if (!engine->database->openOrCreate(name, mode)) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * open database with given name
 *
 * @param database name
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoDatabaseOpen(HSQUIRRELVM v) {
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    if (!engine->database->open(name)) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * close database
 */
SQInteger emoDatabaseClose(HSQUIRRELVM v) {
    if (!engine->database->close()) {
        sq_pushinteger(v, ERR_DATABASE_CLOSE);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * returns database path with given name
 *
 * @param database name
 * @param database path
 */
SQInteger emoDatabaseGetPath(HSQUIRRELVM v) {
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
        sq_poptop(v);
    } else {
        return 0;
    }

    string path  = engine->database->getPath(name);
    const char* cpath = path.c_str();

    sq_pushstring(v, cpath, -1);

    return 1;
}

/*
 * returns latest database error (integer value)
 */
SQInteger emoDatabaseGetLastError(HSQUIRRELVM v) {
    sq_pushinteger(v, engine->database->lastError);
    return 1;
}

/*
 * returns latest database error message
 */
SQInteger emoDatabaseGetLastErrorMessage(HSQUIRRELVM v) {
    const char* str = engine->database->lastErrorMessage.c_str();
    sq_pushstring(v, str, strlen(str));
    return 1;
}

/*
 * open preference database
 */
SQInteger emoDatabaseOpenPreference(HSQUIRRELVM v) {
    if (!engine->database->openDatabase()) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * open or create preference database
 */
SQInteger emoDatabaseOpenOrCreatePreference(HSQUIRRELVM v) {
    if (!engine->database->openOrCreatePreference()) {
        sq_pushinteger(v, ERR_DATABASE_OPEN);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * returns preference value with given key
 *
 * @param preference key
 * @return preference value
 */
SQInteger emoDatabaseGetPreference(HSQUIRRELVM v) {
    const SQChar* key;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &key);
        sq_poptop(v);
    } else {
        return 0;
    }

    string str = engine->database->getPreference(key);
    const char* cstr = str.c_str();

    sq_pushstring(v, cstr, -1);
    return 1;
}

/*
 * set preference value with given key
 *
 * @param preference key
 * @param preference value
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoDatabaseSetPreference(HSQUIRRELVM v) {
    const SQChar* key;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) != OT_NULL) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &key);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    const SQChar* value;
    if (nargs >= 3 && sq_gettype(v, 3) != OT_NULL) {
        sq_tostring(v, 3);
        sq_getstring(v, -1, &value);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }

    if (!engine->database->setPreference(key, value)) {
        sq_pushinteger(v, ERR_DATABASE);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * delete database with given name
 *
 * @param database name
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoDatabaseDeleteDatabase(HSQUIRRELVM v) {
    const SQChar* name;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &name);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
    if (!engine->database->deleteDatabase(name)) {
        sq_pushinteger(v, ERR_DATABASE);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;

}

/*
 * delete preference with given key
 *
 * @param preference key
 * @return EMO_NO_ERROR if succeeds
 */
SQInteger emoDatabaseDeletePreference(HSQUIRRELVM v) {
    const SQChar* key;
    SQInteger nargs = sq_gettop(v);
    if (nargs >= 2 && sq_gettype(v, 2) == OT_STRING) {
        sq_tostring(v, 2);
        sq_getstring(v, -1, &key);
        sq_poptop(v);
    } else {
        sq_pushinteger(v, ERR_INVALID_PARAM);
        return 1;
    }
    if (!engine->database->deletePreference(key)) {
        sq_pushinteger(v, ERR_DATABASE);
        return 1;
    }

    sq_pushinteger(v, EMO_NO_ERROR);
    return 1;
}

/*
 * returns all preference keys
 */
SQInteger emoDatabaseGetPreferenceKeys(HSQUIRRELVM v) {

    vector<emo::CipherHolder> keys = engine->database->getPreferenceKeys();

    sq_newarray(v, 0);

    for (unsigned int i = 0; i < keys.size(); i++) {
        sq_pushstring(v, keys[i].getPlainText().c_str(), -1);
        sq_arrayappend(v, -2);
    }

    sq_push(v, -1);

    return 1;
}

