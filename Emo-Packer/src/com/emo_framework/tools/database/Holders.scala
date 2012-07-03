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

package com.emo_framework.tools.database
import java.util.ArrayList
import org.apache.commons.codec.binary.Base64

class DatabaseHolder() {
    val tables = new ArrayList[TableHolder]();
    def addTable(table: TableHolder) = { tables.add(table) }
    def findTable(name:String):TableHolder = {
        val iteTables = tables.iterator();
        while(iteTables.hasNext()){
            val table = iteTables.next();
            if(table.tableName.getPlainString() == name) return table
        }
        error("findTable : no such table \"" + name + "\"");
    }
}

class TableHolder(tableStr: String, private val key: Array[Byte], val packMode: Boolean, val encryptFlag:Boolean) {
    val tableName = new CipherHolder(tableStr.getBytes(Constants.CHAR_ENCODE), key, packMode, encryptFlag, encryptFlag);
    val columnList = new ArrayList[ColumnHolder]();
    val recordList = new ArrayList[RecordHolder]();

    def addColumn(column: ColumnHolder) = { columnList.add(column) }
    def addRecord(record: RecordHolder) = { recordList.add(record) }
    def clearRecords() = { recordList.clear() };
    def findColumn(name:String):ColumnHolder = {
        val iteColumns = columnList.iterator();
        while(iteColumns.hasNext()){
            val column = iteColumns.next();
            if(column.columnName.getPlainString() == name) return column
        }
        error("findColumn : no such column \"" + name + "\"");
    }
}

class ColumnHolder(columnStr: String, val columnType: String, private val key: Array[Byte], val packMode: Boolean, val encryptFlag: Boolean) {
    val columnName = new CipherHolder(columnStr.getBytes(Constants.CHAR_ENCODE), key, packMode, encryptFlag, encryptFlag);
    val newColumnType = if(columnName.getPlainString() == Constants.TABLE_COLUMN_CONTENT) "BLOB" else "TEXT"
}

class RecordHolder() {
    val valueList = new ArrayList[CipherHolder]();
    def addValue(value: CipherHolder) = { valueList.add(value) }
    def getValue(index: Int) = { valueList.get(index) };
}

class CipherHolder(bytes: Array[Byte], private val key: Array[Byte], val packMode: Boolean, val encryptFlag:Boolean, val base64Flag : Boolean) {
    val plainText: Array[Byte] =
        if (packMode) bytes
        else if (encryptFlag) if (base64Flag) Encryptor.decodeAndDecrypt(bytes, key) else Encryptor.decrypt(bytes, key);
        else if (base64Flag) Base64.decodeBase64(bytes) else bytes;
         
    val cipher: Array[Byte] =
        if (!packMode) bytes
        else if (encryptFlag) if (base64Flag) Encryptor.encryptAndEncode(bytes, key) else Encryptor.encrypt(bytes, key);
        else if (base64Flag) Base64.encodeBase64(bytes) else bytes
        
    def getPlainString() = new String(plainText);
    def getCipherString() = new String(cipher);
}
