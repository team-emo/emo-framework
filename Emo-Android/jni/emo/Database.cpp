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

    if (argc > 0){
        vector<emo::CipherHolder> cols;
        for (int i = 0; i < argc; i++) {
            if(argv[i] == NULL) continue;
            emo::CipherHolder holder = emo::CipherHolder();
            holder.setCipher( string(argv[i]) );
            cols.push_back( holder );
        }
        values->push_back(cols);
    }

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

    CipherHolder::CipherHolder(string text, bool isCipher) {
        if(isCipher == true){
            this->setCipher(text);
        }else{
            this->setPlainText(text);
        }
    }

    CipherHolder::~CipherHolder() {

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

    ColumnHolder::ColumnHolder(CipherHolder name, bool primaryFlag = false){
        this->columnName = name;
        this->isPrimary = primaryFlag;
    }

    ColumnHolder::ColumnHolder(string name, bool primaryFlag = false){
        this->columnName = emo::CipherHolder();
        this->columnName.setPlainText(name);
        this->isPrimary = primaryFlag;
    }

    ColumnHolder::~ColumnHolder(){

    }

    emo::CipherHolder ColumnHolder::getColumnName(){
        return this->columnName;
    }

    bool ColumnHolder::isPrimaryColumn(){
        return this->isPrimary;
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

    ColumnHolder* TableHolder::getColumnHolder(int index){
        return &this->columns.at(index);
    }

    vector<ColumnHolder>* TableHolder::getColumns(){
        return &this->columns;
    }

    CipherHolder TableHolder::getTableName(){
        return this->tableName;
    }

    ConditionBuilder::ConditionBuilder(Database* db, TableHolder* tbl, bool encryptFlag){
        this->database = db;
        this->table = tbl;
        this->condition = string();
        this->isEncrypted = encryptFlag;
        this->isValid = false;
        this->leftParenthCount = 0;
        this->rightParenthCount = 0;
    }

    ConditionBuilder::~ConditionBuilder(){

    }

    bool ConditionBuilder::createCondition(Sqrat::Object condArray){
        for (int i = 0, size = condArray.GetSize(); i < size; i++) {
            if(condArray[i].GetSize() == 1){
                if(condArray[i][0].Cast<std::string>() == "(" || condArray[i][0].Cast<std::string>() == ")"){
                    if(!this->addParenthesis(condArray[i][0].Cast<std::string>())){
                        return false;
                    }
                }else{
                    if(!this->addConjunction(condArray[i][0].Cast<std::string>())){
                        return false;
                    }
                }
            }else if(condArray[i].GetSize() == 3){
                string col    = condArray[i][0].Cast<std::string>();
                string eqSign = condArray[i][1].Cast<std::string>();
                string value  = condArray[i][2].Cast<std::string>();

                if(!this->addCondition(col, eqSign, value)){
                    return false;
                }
            }else{
                LOGE("createCondition : illegal array index for condition");
                return false;
            }
        }
        if(!this->isValidCondition()){
            LOGE("createCondition : invalid condition");
            return false;
        }
        return true;
    }

    bool ConditionBuilder::addCondition(string colName, string eqSign, string value){
        ColumnHolder* column = this->table->getColumnHolder(colName);
        if(this->isValid){
            LOGE("addCondition : bad condition (lack of conjunction).");
            return false;
        }else if(column == NULL){
            LOGE("addCondition : bad condition (no such column).");
            return false;
        }else if(eqSign != "=" && eqSign != "==" && eqSign != "!="){
            LOGE("addCondition : bad condition (illegal equal sign).");
            return false;
        }

        string con;
        if(this->isEncrypted){
            string columnCipher = column->getColumnName().getCipher();
            string tableCipher  = table->getTableName().getCipher();
            vector<CipherHolder> keys;
            int rcode = this->database->select(&keys, columnCipher, tableCipher);
            if(rcode != SQLITE_OK){
                LOGE("addCondition : select failed while creating condition string.");
                return false;
            }
            vector<CipherHolder> targetKeys;
            findCipherHolders(&targetKeys, keys, value);
            if(targetKeys.size() == 0){
                con =  "\"" + columnCipher + "\"" + eqSign + "\"" + value + "\"";
            }else{
                con = "(";
                bool firstFlag = true;
                vector<emo::CipherHolder>::iterator itKeys;
                for(itKeys = targetKeys.begin(); itKeys != targetKeys.end(); itKeys++){
                    if(firstFlag){
                        firstFlag = false;
                    }else{
                        if(eqSign == "!="){
                            con += " AND ";
                        }else{
                            con += " OR ";
                        }
                    }
                    con += "\"" + columnCipher + "\" " + eqSign + " \"" + itKeys->getCipher() + "\"";
                }
                con += ")";
            }
        }else{
            con = "\"" + colName + "\" " + eqSign + " \"" + value + "\"";
        }
        this->condition += con;
        this->isValid = true;
        return true;
    }

    bool ConditionBuilder::addConjunction(string conjunction){
        if(!this->isValid){
            LOGE("addConjunction : unnecessary conjunction.");
            return false;
        }else if(conjunction != "AND" && conjunction != "and" && conjunction != "OR" && conjunction != "or"){
            LOGE("addConjunction : illegal conjunction.");
            return false;
        }
        this->condition += " " + conjunction + " ";
        this->isValid = false;
        return true;
    }

    bool ConditionBuilder::addParenthesis(string parenthesis){
        if(parenthesis == "("){
            if(this->isValid){
                LOGE("addParenthesis : invalid parenthesis.");
                return false;
            }
            this->leftParenthCount++;
        }else if(parenthesis == ")"){
            if(!this->isValid){
                LOGE("addParenthesis : invalid parenthesis.");
                return false;
            }
            this->rightParenthCount++;
        }else{
            LOGE("addParenthesis : illegal character.");
            return false;
        }
        this->condition += parenthesis;
        return true;
    }

    string ConditionBuilder::getCondition(){
        return this->condition;
    }

    bool ConditionBuilder::isValidCondition(){
        if( !this->isValid || this->leftParenthCount != this->rightParenthCount){
            return false;
        }
        return true;
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

    string Database::getPath(string name = DEFAULT_DATABASE_NAME) {
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

    string Database::create(string name = DEFAULT_DATABASE_NAME, jint mode = FILE_MODE_PRIVATE) {
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

    bool Database::remove(string name = DEFAULT_DATABASE_NAME) {
        JNIEnv* env;
        JavaVM* vm = engine->app->activity->vm;
        
        vm->AttachCurrentThread(&env, NULL);

        jclass clazz = env->GetObjectClass(engine->app->activity->clazz);
        jmethodID methodj = env->GetMethodID(clazz, "deleteDatabase", "(Ljava/lang/String;)Z");
        jboolean deleted = env->CallBooleanMethod(engine->app->activity->clazz, methodj, env->NewStringUTF(name.c_str()));

        vm->DetachCurrentThread();

        return deleted;
    }

    bool Database::openOrCreate(string name = DEFAULT_DATABASE_NAME, jint mode = FILE_MODE_PRIVATE, bool encryptFlag = true) {
        if (this->isOpen) return true;

        string path = this->create(name, mode);

        int rcode = sqlite3_open(path.c_str(), &this->db);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage =  sqlite3_errmsg(this->db);
            return false;
        }

        // encryption could be switched only when tables are not packed
        if(engine->config->tablesInfo.packed == false){
            engine->config->tablesInfo.encrypted = encryptFlag;
        }

        this->isOpen = true;
        // analyze DB to get names of all the tables and columns
        if(!this->isAnalyzed) this->analyze();

        return true;
    }

    bool Database::open(string name = DEFAULT_DATABASE_NAME, bool encryptFlag = true) {
        if (this->isOpen) return false;

        string path = this->getPath(name);

        int rcode = sqlite3_open(path.c_str(), &this->db);
        if (rcode != SQLITE_OK) {
            this->lastError = rcode;
            this->lastErrorMessage =  sqlite3_errmsg(this->db);
            return false;
        }

        // encryption could be switched only when tables are not packed
        if(engine->config->tablesInfo.packed == false){
            engine->config->tablesInfo.encrypted = encryptFlag;
        }

        this->isOpen = true;
        if(!this->isAnalyzed) this->analyze();

        return true;
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
                bool primaryFlag = false;
                if(itCols->at(4).getPlainText() == "1"){
                    primaryFlag = true;
                }
                emo::ColumnHolder column = emo::ColumnHolder(itCols->at(1), primaryFlag);
                table.addColumn(column);
            }
            this->tables->push_back(table);
        }
        this->isAnalyzed = true;
    }

    void Database::addTableHolder(TableHolder& table){
        this->tables->push_back(table);
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

    bool Database::isDuplicatedKey(string& tableName, int index, CipherHolder& value, bool encryptFlag){
        TableHolder* table = this->getTableHolder(tableName);
        ColumnHolder* targetColumn = table->getColumnHolder(index);
        if(!targetColumn->isPrimaryColumn()){
            return false;
        }

        vector<CipherHolder> values;
        if(encryptFlag == true){
            this->select(&values, targetColumn->getColumnName().getCipher(), table->getTableName().getCipher());
        }else{
            this->select(&values, targetColumn->getColumnName().getPlainText(), tableName);
        }

        CipherHolder detectedValue= CipherHolder();
        return findCipherHolder(&detectedValue, values, value.getPlainText());
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

    int Database::selectWhere(vector<emo::CipherHolder>* values, const string& targetColumn, const string& tableName, ConditionBuilder& con){
        string sqlString = "SELECT \"" + targetColumn + "\" FROM '" + tableName + "' WHERE " + con.getCondition();

        int rcode = this->query((char*)sqlString.c_str(), database_callback_string_cipher, values, false);
        return rcode;
    }

    int Database::selectAll(vector< vector<emo::CipherHolder> >* values, const string& tableName){
        char *sql = sqlite3_mprintf( "SELECT * FROM %Q", tableName.c_str() );

        int rcode = this->query(sql, database_callback_multiple_string_cipher, values, true);
        return rcode;
    }

    int Database::selectAllWhere(vector< vector<emo::CipherHolder> >* values, const string& tableName, ConditionBuilder& con){
        string sqlString = "SELECT * FROM '" + tableName + "' WHERE " + con.getCondition();

        int rcode = this->query((char*)sqlString.c_str(), database_callback_multiple_string_cipher, values, false);
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

    int Database::countWhere(int* value, const string& tableName, ConditionBuilder& con){
        string sqlString = "SELECT COUNT(*) FROM " + tableName + " WHERE " + con.getCondition();

        int rcode = this->query((char*)sqlString.c_str(), database_callback_count, value, false);
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

    int Database::update(const string& tableName, vector< vector<string> >& columns){
        string stringValues = string("");

        bool firstFlag = true;
        vector< vector<string> >::iterator itColumns;
        for(itColumns = columns.begin(); itColumns != columns.end(); itColumns++){
            if(firstFlag == true){
                firstFlag = false;
            }else{
                stringValues += ", ";
            }
            stringValues += "\"" + itColumns->at(0) + "\"" + " = " + "\"" + itColumns->at(1) + "\"";
        }
        string sqlString = "UPDATE '" + tableName + "' SET " + stringValues;

        return this->exec((char*)sqlString.c_str(), false);
    }

    int Database::updateWhere(const string& tableName, vector< vector<string> >& columns, ConditionBuilder& con){
        string stringValues = string("");

        bool firstFlag = true;
        vector< vector<string> >::iterator itColumns;
        for(itColumns = columns.begin(); itColumns != columns.end(); itColumns++){
            if(firstFlag == true){
                firstFlag = false;
            }else{
                stringValues += ", ";
            }
            stringValues += "\"" + itColumns->at(0) + "\"" + " = " + "\"" + itColumns->at(1) + "\"";
        }
        string sqlString = "UPDATE '" + tableName + "' SET " + stringValues + " WHERE " + con.getCondition();

        return this->exec((char*)sqlString.c_str(), false);
    }

    int Database::deleteWhere(const string& tableName, ConditionBuilder& con){
        string sqlString = "DELETE FROM '" + tableName + "' WHERE " + con.getCondition();

        return this->exec((char*)sqlString.c_str(), false);
    }

    int Database::truncateTable(const string& tableName){
        char *sql = sqlite3_mprintf("DELETE FROM %Q", tableName.c_str());

        return this->exec(sql, true);
    }

    int Database::vacuum(){
        char sql [8] = "VACUUM";
        return this->exec(sql, false);
    }

    bool Database::openOrCreatePreference(bool encryptFlag) {
        bool result = this->openOrCreate(DEFAULT_DATABASE_NAME, FILE_MODE_PRIVATE, encryptFlag);
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
        int rcode;
        if(engine->config->tablesInfo.encrypted){
            columnNames.push_back(keyHolder.getCipher());
            columnNames.push_back(valueHolder.getCipher());
            rcode = this->createTable(tableName.getCipher(), columnNames, true);
        }else{
            columnNames.push_back(keyHolder.getPlainText());
            columnNames.push_back(valueHolder.getPlainText());
            rcode = this->createTable(tableName.getPlainText(), columnNames, true);
        }

        if(rcode == SQLITE_OK){
            emo::TableHolder table = emo::TableHolder(tableName);
            table.addColumn(emo::ColumnHolder(keyHolder, true));
            table.addColumn(emo::ColumnHolder(valueHolder, false));
            this->tables->push_back(table);
        }
        return rcode == SQLITE_OK;
    }

    string Database::getPreference(string key) {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference(engine->config->tablesInfo.encrypted);
            forceClose = true;
        }
        vector<emo::CipherHolder> keyList = getPreferenceKeys();
        CipherHolder keyHolder;
        if(findCipherHolder(&keyHolder, keyList, key) == false){
            return string();
        }
        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);
        ColumnHolder* valueColumn     = preferenceTable->getColumnHolder("VALUE");
        ConditionBuilder condition(this, preferenceTable, engine->config->tablesInfo.encrypted);
        condition.addCondition("KEY", "==", key);

        vector<emo::CipherHolder> values;
        if(engine->config->tablesInfo.encrypted){
            this->selectWhere(
                    &values,
                    valueColumn->getColumnName().getCipher(),
                    preferenceTable->getTableName().getCipher(),
                    condition );
        }else{
            this->selectWhere(
                    &values,
                    valueColumn->getColumnName().getPlainText(),
                    preferenceTable->getTableName().getPlainText(),
                    condition );
        }

        if (forceClose) {
            this->close();
        }
        return values.at(0).getPlainText();
    }

    bool Database::setPreference(string key, string value) {
        if(key.empty()) return false;

        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference(engine->config->tablesInfo.encrypted);
            forceClose = true;
        }
        vector<emo::CipherHolder> keyList = getPreferenceKeys();
        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);

        // encrypt before storing to database
        string encryptedValue = encryptString(value);

        int rcode = SQLITE_OK;
        CipherHolder keyHolder;
        if(findCipherHolder(&keyHolder, keyList, key) == false){
            vector<string> values;
            if(engine->config->tablesInfo.encrypted){
                string encryptedKey = encryptString(key);
                values.push_back(encryptedKey);
                values.push_back(encryptedValue);
                rcode = this->insert(preferenceTable->getTableName().getCipher(), values);
            }else{
                values.push_back(key);
                values.push_back(value);
                rcode = this->insert(preferenceTable->getTableName().getPlainText(), values);
            }
        }else{
            ColumnHolder* valueColumn     = preferenceTable->getColumnHolder("VALUE");
            vector< vector<string> > columns;
            vector<string> colAndValue;
            ConditionBuilder condition(this, preferenceTable, engine->config->tablesInfo.encrypted);
            condition.addCondition("KEY", "==", key);

            if(engine->config->tablesInfo.encrypted){
                colAndValue.push_back(valueColumn->getColumnName().getCipher());
                colAndValue.push_back(encryptedValue);
                columns.push_back(colAndValue);

                rcode = this->updateWhere(preferenceTable->getTableName().getCipher(),
                        columns, condition);
            }else{
                colAndValue.push_back(valueColumn->getColumnName().getPlainText());
                colAndValue.push_back(value);
                columns.push_back(colAndValue);

                rcode = this->updateWhere(preferenceTable->getTableName().getPlainText(),
                        columns, condition);
            }
        }

        if (forceClose) {
            this->close();
        }
        return rcode == SQLITE_OK;
    }

    vector<emo::CipherHolder> Database::getPreferenceKeys() {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference(engine->config->tablesInfo.encrypted);
            forceClose = true;
        }
        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);
        ColumnHolder* keyColumn       = preferenceTable->getColumnHolder("KEY");

        vector<emo::CipherHolder> keys;
        if(engine->config->tablesInfo.encrypted){
            this->select(&keys,
                    keyColumn->getColumnName().getCipher(),
                    preferenceTable->getTableName().getCipher() );
        }else{
            this->select(&keys,
                    keyColumn->getColumnName().getPlainText(),
                    preferenceTable->getTableName().getPlainText() );
        }

        if (forceClose) {
            this->close();
        }
        return keys;
    }

    bool Database::deletePreference(string key) {
        bool forceClose = false;
        if (!this->isOpen) {
            this->openOrCreatePreference(engine->config->tablesInfo.encrypted);
            forceClose = true;
        }
        vector<emo::CipherHolder> keyList = getPreferenceKeys();

        TableHolder* preferenceTable  = this->getTableHolder(PREFERENCE_TABLE_NAME);
        ConditionBuilder condition(this, preferenceTable, engine->config->tablesInfo.encrypted);
        condition.addCondition("KEY", "==", key);

        int rcode;
        if(engine->config->tablesInfo.encrypted){
            rcode = this->deleteWhere(
                    preferenceTable->getTableName().getCipher(),
                    condition);
        }else{
            rcode = this->deleteWhere(
                    preferenceTable->getTableName().getPlainText(),
                    condition);
        }

        if (forceClose) {
            this->close();
        }
        return rcode == SQLITE_OK;
    }

    char* Database::getConfig() {
        bool forceClose = false;
        if (!this->isOpen) {
            this->open();
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
            this->open();
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

// **************************************************
// *                Native Closures                 *
// **************************************************

/*
 * open database with given name
 * if database is not found, creates new database with given name
 * file mode is one of FILE_MODE_PRIVATE, FILE_MODE_WORLD_READABLE
 * or FILE_MODE_WORLD_WRITABLE
 *
 * @param encryption flag [default : true]
 * @param database name [default : DEFAULT_DATABASE_NAME]
 * @param file mode [default : FILE_MODE_PRIVATE]
 * @return EMO_NO_ERROR if succeeds
 */
int sqOpenOrCreate(bool encryptFlag, string databaseName, int mode) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount > 3) {
        LOGE("sqOpenOrCreate : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    if (paramCount < 3) {
        mode = FILE_MODE_PRIVATE;
    }
    if (paramCount < 2) {
        databaseName = DEFAULT_DATABASE_NAME;
    }
    if (paramCount < 1) {
        encryptFlag = true;
    }

    // check file mode
    if (mode != FILE_MODE_PRIVATE && mode != FILE_MODE_WORLD_READABLE
            && mode != FILE_MODE_WORLD_WRITEABLE) {
        LOGE("sqOpenOrCreate : invalid file mode specified");
        return ERR_INVALID_PARAM;
    }

    if (engine->database->openOrCreate(databaseName, mode, encryptFlag) == false) {
        return ERR_DATABASE_OPEN;
    }
    return EMO_NO_ERROR;
}

/*
 * open database with given name
 *
 * @param encryption flag [default : true]
 * @param database name [default : DEFAULT_DATABASE_NAME]
 * @return EMO_NO_ERROR if succeeds
 */
int sqOpen(bool encryptFlag, string databaseName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount > 2) {
        LOGE("sqOpen : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    if (paramCount < 2) {
        databaseName = DEFAULT_DATABASE_NAME;
    }
    if (paramCount < 1) {
        encryptFlag = true;
    }

    if (engine->database->open(databaseName, encryptFlag) == false) {
        return ERR_DATABASE_OPEN;
    }
    return EMO_NO_ERROR;
}

/*
 * close database
 *
 * @return EMO_NO_ERROR if succeeds
 */
int sqClose() {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 0) {
        LOGE("sqClose : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }

    if (engine->database->close() == false) {
        return ERR_DATABASE_CLOSE;
    }
    return EMO_NO_ERROR;
}

/*
 * remove database with given name
 *
 * @param database name [default : DEFAULT_DATABASE_NAME]
 * @return EMO_NO_ERROR if succeeds
 */
int sqRemove(string databaseName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount > 1) {
        LOGE("sqRemove : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    if (paramCount < 1) {
        databaseName = DEFAULT_DATABASE_NAME;
    }

    if (engine->database->remove(databaseName) == false) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * create table with given column names
 *
 * @param table name
 * @param column names
 * @param primary key flag
 * @return EMO_NO_ERROR if succeeds
 */
int sqCreateTable(string tableName, Sqrat::Object columnNames, bool primaryFlag) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 3) {
        LOGE("sqCreateTable : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    if (engine->database->getTableHolder(tableName) != NULL) {
        LOGE("sqCreateTable : table with given name already exists");
        return ERR_INVALID_PARAM;
    }
    bool encryptFlag = engine->config->tablesInfo.encrypted;

    emo::CipherHolder tableNameCipher = emo::CipherHolder(tableName,
            !encryptFlag);
    emo::TableHolder tableHolder = emo::TableHolder(tableNameCipher);
    vector<emo::CipherHolder> columns;
    for (int i = 0, size = columnNames.GetSize(); i < size; i++) {
        emo::CipherHolder colName = emo::CipherHolder(columnNames[i].Cast<string>(), !encryptFlag);
        columns.push_back(colName);

        bool pFlag = false;
        if (i == 0) {
            pFlag = primaryFlag;
        }
        emo::ColumnHolder colHolder = emo::ColumnHolder(colName, pFlag);
        tableHolder.addColumn(colHolder);
    }

    int rcode;
    vector < string > colStrings;
    if (encryptFlag) {
        getCiphers(&colStrings, columns);
        rcode = engine->database->createTable(tableNameCipher.getCipher(), colStrings, primaryFlag);
    } else {
        getPlainTexts(&colStrings, columns);
        rcode = engine->database->createTable(tableName, colStrings, primaryFlag);
    }
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    } else {
        engine->database->addTableHolder(tableHolder);
    }
    return EMO_NO_ERROR;
}

/*
 * drop table with given name
 *
 * @param table name
 * @return EMO_NO_ERROR if succeeds
 */
int sqDropTable(string tableName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 1) {
        LOGE("sqDropTable : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqDropTable : table with given name not exists");
        return ERR_INVALID_PARAM;
    }

    int rcode;
    if (engine->config->tablesInfo.encrypted) {
        rcode = engine->database->dropTable(table->getTableName().getCipher());
    } else {
        rcode = engine->database->dropTable(tableName);
    }
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * select values with given condition
 *
 * @param target column name
 * @param table name
 * @return selected values (array)
 */
Sqrat::Object sqSelect(string targetColumn, string tableName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 2) {
        LOGE("sqSelect : wrong number of arguments");
        return Sqrat::Array();
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqSelect : no such table");
        return Sqrat::Array();
    }
    emo::ColumnHolder* column = table->getColumnHolder(targetColumn);
    if (column == NULL) {
        LOGE("sqSelect : no such column");
        return Sqrat::Array();
    }

    int rcode;
    vector<emo::CipherHolder> values;
    if (engine->config->tablesInfo.encrypted) {
        string targetTableCipher = table->getTableName().getCipher();
        string targetColumnCipher = column->getColumnName().getCipher();
        rcode = engine->database->select(&values, targetColumnCipher, targetTableCipher);
    } else {
        rcode = engine->database->select(&values, targetColumn, tableName);
    }
    if (rcode != SQLITE_OK) {
        LOGE("sqSelect : SQL statement, 'select [columnName]', failed");
        return Sqrat::Array();
    }

    Sqrat::Array array(engine->sqvm, values.size());
    for (unsigned int i = 0; i < values.size(); i++) {
        array.SetValue(i, values[i].getPlainText());
    }
    return array;
}

/*
 * select all values with given condition
 *
 * @param table name
 * @return selected records (2d array)
 */
Sqrat::Object sqSelectAll(string tableName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 1) {
        LOGE("sqSelectAll : wrong number of arguments");
        return Sqrat::Array();
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqSelectAll : no such table");
        return Sqrat::Array();
    }

    int rcode;
    vector < vector<emo::CipherHolder> > records;
    if (engine->config->tablesInfo.encrypted) {
        string targetTableCipher = table->getTableName().getCipher();
        rcode = engine->database->selectAll(&records, targetTableCipher);
    } else {
        rcode = engine->database->selectAll(&records, tableName);
    }
    if (rcode != SQLITE_OK) {
        return Sqrat::Array();
    }

    Sqrat::Array array(engine->sqvm, records.size());
    for (unsigned int i = 0; i < records.size(); i++) {
        Sqrat::Array array2(engine->sqvm, records[i].size());
        for (unsigned int j = 0; j < records[i].size(); j++) {
            array2.SetValue(j, records[i][j].getPlainText());
        }
        array.Bind(i, array2);
    }
    return array;
}

/*
 * select values with given condition
 *
 * @param target column name
 * @param table name
 * @param 2D array with given condition
 * @return selected values (array)
 */
Sqrat::Object sqSelectWhere(string targetColName, string tableName, Sqrat::Object conds) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 3) {
        LOGE("sqSelectWhere : wrong number of arguments");
        return Sqrat::Array();
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqSelectWhere : no such table");
        return Sqrat::Array();
    }
    emo::ColumnHolder* targetColumn = table->getColumnHolder(targetColName);
    if (targetColumn == NULL) {
        LOGE("sqSelectWhere : no such column (target)");
        return Sqrat::Array();
    }
    emo::ConditionBuilder conBuilder(engine->database, table, engine->config->tablesInfo.encrypted);
    if (!conBuilder.createCondition(conds)) {
        return Sqrat::Array();
    }

    int rcode;
    vector<emo::CipherHolder> values;
    if (engine->config->tablesInfo.encrypted) {
        string targetTableCipher = table->getTableName().getCipher();
        string targetColumnCipher = targetColumn->getColumnName().getCipher();
        rcode = engine->database->selectWhere(&values, targetColumnCipher, targetTableCipher, conBuilder);
    } else {
        rcode = engine->database->selectWhere(&values, targetColName, tableName, conBuilder);
    }
    if (rcode != SQLITE_OK) {
        return Sqrat::Array();
    }

    Sqrat::Array array(engine->sqvm, values.size());
    for (unsigned int i = 0; i < values.size(); i++) {
        array.SetValue(i, values[i].getPlainText());
    }
    return array;
}

/*
 * select records with given condition
 *
 * @param table name
 * @param 2D array with given condition
 * @return selected records (table)
 */
Sqrat::Object sqSelectAllWhere(string tableName, Sqrat::Object conds) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 2) {
        LOGE("sqSelectAllWhere : wrong number of arguments");
        return Sqrat::Array();
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqSelectAllWhere : no such table");
        return Sqrat::Array();
    }
    emo::ConditionBuilder conBuilder(engine->database, table, engine->config->tablesInfo.encrypted);
    if (!conBuilder.createCondition(conds)) {
        return Sqrat::Array();
    }

    int rcode;
    vector < vector<emo::CipherHolder> > records;
    if (engine->config->tablesInfo.encrypted) {
        string targetTableCipher = table->getTableName().getCipher();
        rcode = engine->database->selectAllWhere(&records, targetTableCipher, conBuilder);
    } else {
        rcode = engine->database->selectAllWhere(&records, tableName, conBuilder);
    }
    if (rcode != SQLITE_OK) {
        return Sqrat::Array();
    }

    Sqrat::Array array(engine->sqvm, records.size());
    for (unsigned int i = 0; i < records.size(); i++) {
        Sqrat::Array array2(engine->sqvm, records[i].size());
        for (unsigned int j = 0; j < records[i].size(); j++) {
            array2.SetValue(j, records[i][j].getPlainText());
        }
        array.Bind(i, array2);
    }
    return array;
}

/*
 * count records on specified table with given condition
 *
 * @param table name
 * @return count of records or -1 on failure
 */
int sqCount(string tableName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 1) {
        LOGE("sqCount : wrong number of arguments");
        return -1;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqCount : no such table");
        return -1;
    }

    int rcode, count;
    if (engine->config->tablesInfo.encrypted) {
        rcode = engine->database->count(&count, table->getTableName().getCipher());
    } else {
        rcode = engine->database->count(&count, tableName);
    }
    if (rcode != SQLITE_OK) {
        return -1;
    }
    return count;
}

/*
 * count records on specified table
 *
 * @param table name
 * @param 2D array with given condition
 * @return count of matched records
 */
int sqCountWhere(string tableName, Sqrat::Object conds) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 2) {
        LOGE("sqCountWhere : wrong number of arguments");
        return -1;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqCountWhere : no such table");
        return -1;
    }
    emo::ConditionBuilder conBuilder(engine->database, table, engine->config->tablesInfo.encrypted);
    if (!conBuilder.createCondition(conds)) {
        return -1;
    }

    int rcode, count;
    if (engine->config->tablesInfo.encrypted) {
        rcode = engine->database->countWhere(&count, table->getTableName().getCipher(), conBuilder);
    } else {
        rcode = engine->database->countWhere(&count, tableName, conBuilder);
    }
    if (rcode != SQLITE_OK) {
        return -1;
    }
    return count;
}

/*
 * update records with given values
 *
 * @param table name
 * @param values to insert (array)
 * @return EMO_NO_ERROR if succeeds
 */
int sqInsert(string tableName, Sqrat::Object sqValues) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 2) {
        LOGE("sqInsert : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqInsert : no such table");
        return ERR_INVALID_PARAM;
    }
    if ((unsigned int) sqValues.GetSize() != table->getColumns()->size()) {
        LOGE("sqInsert : number of values does not match");
        return ERR_INVALID_PARAM;
    }
    bool encryptFlag = engine->config->tablesInfo.encrypted;

    vector<emo::CipherHolder> holders;
    for (int i = 0, size = sqValues.GetSize(); i < size; i++) {
        emo::CipherHolder holder = emo::CipherHolder(sqValues[i].Cast<std::string>(), !encryptFlag);
        if (engine->database->isDuplicatedKey(tableName, i, holder, encryptFlag)) {
            LOGE("sqInsert : duplicated primary key");
            return ERR_INVALID_PARAM;
        }
        holders.push_back(holder);
    }

    int rcode;
    vector < string > values;
    if (engine->config->tablesInfo.encrypted) {
        getCiphers(&values, holders);
        rcode = engine->database->insert(table->getTableName().getCipher(), values);
    } else {
        getPlainTexts(&values, holders);
        rcode = engine->database->insert(tableName, values);
    }
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * update records with given values
 *
 * @param table name
 * @param 2D array with target column & value pairs
 * @return EMO_NO_ERROR if succeeds
 */
int sqUpdate(string tableName, Sqrat::Object dataSets) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 2) {
        LOGE("sqUpdate : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqUpdate : no such table");
        return ERR_INVALID_PARAM;
    }

    vector < vector<string> > columns;
    for (int i = 0, size = dataSets.GetSize(); i < size; i++) {
        if (dataSets[i].GetSize() != 2) {
            LOGE("sqUpdate : illegal array index for data sets");
            return ERR_INVALID_PARAM;
        }
        vector < string > colAndValue;
        string targetColName = dataSets[i][0].Cast<std::string>();
        string targetValue = dataSets[i][1].Cast<std::string>();

        emo::ColumnHolder* targetColumn = table->getColumnHolder(targetColName);
        if (targetColumn == NULL) {
            LOGE("sqUpdate : no such column (target)");
            return ERR_INVALID_PARAM;
        }
        if (targetColumn->isPrimaryColumn()) {
            LOGE("sqUpdate : unable to update primary key");
            return ERR_INVALID_PARAM;
        }
        if (engine->config->tablesInfo.encrypted) {
            colAndValue.push_back(targetColumn->getColumnName().getCipher());
            colAndValue.push_back(encryptString(targetValue));
        } else {
            colAndValue.push_back(targetColName);
            colAndValue.push_back(targetValue);
        }
        columns.push_back(colAndValue);
    }

    int rcode;
    if (engine->config->tablesInfo.encrypted) {
        rcode = engine->database->update(table->getTableName().getCipher(), columns);
    } else {
        rcode = engine->database->update(tableName, columns);
    }
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * update records with given values and condition
 *
 * @param table name
 * @param 2D array with target column & value pairs
 * @param 2D array with given conditions
 * @return EMO_NO_ERROR if succeeds
 */
int sqUpdateWhere(string tableName, Sqrat::Object dataSets, Sqrat::Object conds) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 3) {
        LOGE("sqUpdateWhere : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqUpdateWhere : no such table");
        return ERR_INVALID_PARAM;
    }
    emo::ConditionBuilder conBuilder(engine->database, table, engine->config->tablesInfo.encrypted);
    if (!conBuilder.createCondition(conds)) {
        return ERR_DATABASE;
    }

    vector < vector<string> > columns;
    for (int i = 0, size = dataSets.GetSize(); i < size; i++) {
        if (dataSets[i].GetSize() != 2) {
            LOGE("sqUpdateWhere : illegal array index for data sets");
            return ERR_INVALID_PARAM;
        }
        vector < string > colAndValue;
        string targetColName = dataSets[i][0].Cast<std::string>();
        string targetValue = dataSets[i][1].Cast<std::string>();

        emo::ColumnHolder* targetColumn = table->getColumnHolder(targetColName);
        if (targetColumn == NULL) {
            LOGE("sqUpdateWhere : no such column (target)");
            return ERR_INVALID_PARAM;
        }
        if (targetColumn->isPrimaryColumn()) {
            LOGE("sqUpdateWhere : unable to update primary key");
            return ERR_INVALID_PARAM;
        }
        if (engine->config->tablesInfo.encrypted) {
            colAndValue.push_back(targetColumn->getColumnName().getCipher());
            colAndValue.push_back(encryptString(targetValue));
        } else {
            colAndValue.push_back(targetColName);
            colAndValue.push_back(targetValue);
        }
        columns.push_back(colAndValue);
    }

    int rcode;
    if (engine->config->tablesInfo.encrypted) {
        rcode = engine->database->updateWhere(table->getTableName().getCipher(), columns, conBuilder);
    } else {
        rcode = engine->database->updateWhere(tableName, columns, conBuilder);
    }
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * delete all records of the table
 *
 * @param table name
 * @return EMO_NO_ERROR if succeeds
 */
int sqTruncateTable(string tableName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 1) {
        LOGE("sqTruncate : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqTruncate : no such table");
        return ERR_INVALID_PARAM;
    }

    int rcode;
    if (engine->config->tablesInfo.encrypted) {
        rcode = engine->database->truncateTable(table->getTableName().getCipher());
    } else {
        rcode = engine->database->truncateTable(tableName);
    }
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * delete records with given condition
 *
 * @param table name
 * @param 2D array with given conditions
 * @return EMO_NO_ERROR if succeeds
 */
int sqDeleteWhere(string tableName, Sqrat::Object conds) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 2) {
        LOGE("sqDeleteWhere : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    emo::TableHolder* table = engine->database->getTableHolder(tableName);
    if (table == NULL) {
        LOGE("sqDeleteWhere : no such table");
        return ERR_INVALID_PARAM;
    }
    emo::ConditionBuilder conBuilder(engine->database, table, engine->config->tablesInfo.encrypted);
    if (!conBuilder.createCondition(conds)) {
        return ERR_DATABASE;
    }

    int rcode;
    if (engine->config->tablesInfo.encrypted) {
        rcode = engine->database->deleteWhere(table->getTableName().getCipher(), conBuilder);
    } else {
        rcode = engine->database->deleteWhere(tableName, conBuilder);
    }
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * vacuum unnecessary data from database
 *
 * @return EMO_NO_ERROR if succeeds
 */
int sqVacuum() {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 0) {
        LOGE("sqVacuum : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }

    int rcode = engine->database->vacuum();
    if (rcode != SQLITE_OK) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * returns database path with given name
 *
 * @param database name [default : DEFAULT_DATABASE_NAME]
 * @return database path
 */
string sqGetPath(string databaseName) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount > 1) {
        LOGE("sqGetPath : wrong number of arguments");
        return string();
    }
    if (paramCount < 1) {
        databaseName = DEFAULT_DATABASE_NAME;
    }

    return engine->database->getPath(databaseName);
}

/*
 * returns latest database error (integer value)
 *
 * @return last database error
 */
int sqGetLastError() {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 0) {
        LOGE("sqGetLastError : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }

    return engine->database->lastError;
}

/*
 * returns latest database error message
 *
 * @return last database error message
 */
string sqGetLastErrorMessage() {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 0) {
        LOGE("sqGetLastErrorMessage : wrong number of arguments");
        return string();
    }

    return engine->database->lastErrorMessage;
}

/*
 * open or create preference database
 *
 * @param encryption flag [default : true]
 * @return true when successful
 */
int sqOpenOrCreatePreference(bool encryptFlag) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount > 1) {
        LOGE("sqOpenOrCreatePreference : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }
    if (paramCount < 1) {
        encryptFlag = true;
    }
    if (!engine->database->openOrCreatePreference(encryptFlag)) {
        return ERR_DATABASE_OPEN;
    }
    return EMO_NO_ERROR;
}

/*
 * returns preference value with given key
 *
 * @param preference key
 * @return preference value
 */
string sqGetPreference(string key) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 1) {
        LOGE("sqGetPreference : wrong number of arguments");
        return string();
    }

    return engine->database->getPreference(key);
}

/*
 * set preference value with given key
 *
 * @param preference key
 * @param preference value
 * @return EMO_NO_ERROR if succeeds
 */
int sqSetPreference(string key, string value) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 2) {
        LOGE("sqSetPreference : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }

    if (!engine->database->setPreference(key, value)) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * delete preference with given key
 *
 * @param preference key
 * @return EMO_NO_ERROR if succeeds
 */
int sqDeletePreference(string key) {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 1) {
        LOGE("sqDeletePreference : wrong number of arguments");
        return ERR_INVALID_PARAM;
    }

    if (!engine->database->deletePreference(key)) {
        return ERR_DATABASE;
    }
    return EMO_NO_ERROR;
}

/*
 * returns all preference keys
 */
Sqrat::Object sqGetPreferenceKeys() {
    // param check
    int paramCount = emo::SquirrelGlue::getParamCount(engine->sqvm);
    if (paramCount != 0) {
        LOGE("sqGetPreferenceKeys : wrong number of arguments");
        return Sqrat::Array();
    }

    vector<emo::CipherHolder> keys = engine->database->getPreferenceKeys();
    Sqrat::Array array(engine->sqvm, keys.size());
    for (unsigned int i = 0; i < keys.size(); i++) {
        array.SetValue(i, keys[i].getPlainText());
    }
    return array;
}

