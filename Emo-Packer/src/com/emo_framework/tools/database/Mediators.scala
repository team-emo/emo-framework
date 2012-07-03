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
import java.net.URI
import scala.dbc.Vendor
import scala.dbc.Database
import java.sql.DriverManager
import java.sql.Connection
import java.util.ArrayList
import java.sql.PreparedStatement
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import org.apache.commons.io.FileUtils
import java.io.OutputStream
import org.apache.commons.io.output.ByteArrayOutputStream
import org.apache.log4j.Logger
import org.apache.log4j.BasicConfigurator
import org.apache.log4j.FileAppender
import org.apache.log4j.Level
import org.apache.log4j.PatternLayout

class SQLiteMediator(val path: String, val dataHolder: DatabaseHolder, val target:String) extends LogWriter{
    Class.forName("org.sqlite.JDBC");
    val key = Config.key;
    val database = DriverManager.getConnection("jdbc:sqlite:" + path)
    val encryptFlag = Config.getEncryptFlag(target);
    
    def buildTableHolders() = {
        logger.info("Building all \"" + target + "\"");
        val tableIte = getTableList().iterator();
        while (tableIte.hasNext()) {
            val tableName = tableIte.next();
            val copyFlag = Config.isCopyTable( tableName );
            if( !Config.isExceptionTable(tableName) || copyFlag){
                val tableHolder = getTableHolder(tableName, copyFlag);
                dataHolder.addTable(tableHolder);
            }
        }
    }

    def getTableList(): ArrayList[String] = {
        val tableList = new ArrayList[String]();
        val stmt = database.createStatement
        val sql = "SELECT name FROM sqlite_master WHERE type='table'"
        logger.info("SQL : " + sql);
        val res = stmt.executeQuery(sql)

        while (res.next) {
            val tableName = res.getString(1);
            tableList.add(tableName);
        }

        stmt.close();
        return tableList;
    }

    def getTableHolder(tableName: String, copyFlag: Boolean): TableHolder = {
        val tableEncryptFlag = if (copyFlag) false else encryptFlag;
        val tableHolder = new TableHolder(tableName, key, Config.packModeFlag, tableEncryptFlag);
        val stmt = database.createStatement
        val sql = "PRAGMA TABLE_INFO('" + tableName + "')";
        logger.info("SQL : " + sql);
        val res = stmt.executeQuery(sql)

        while (res.next) {
            val index = res.getInt(1);
            val columnName = res.getString(2);
            val columnType = res.getString(3);
            val notNull = res.getBoolean(4);
            val defaultValue = res.getString(5);
            val columnHolder = new ColumnHolder(columnName, columnType, key, Config.packModeFlag, tableEncryptFlag);
            tableHolder.addColumn(columnHolder);
        }

        stmt.close();
        return tableHolder;
    }

    def createTables() {
        val tableIte = dataHolder.tables.iterator();
        while (tableIte.hasNext()) {
            val tableHolder = tableIte.next();
            val tableName = tableHolder.tableName.getPlainString();
            if( !Config.isReservedTable(tableName) || Config.packModeFlag ){
                createTable(tableHolder);
            }
        }
    }

    def createTable(tableHolder: TableHolder) {
        val tableIte = dataHolder.tables.iterator();
        val tableName = if (Config.packModeFlag && tableHolder.encryptFlag) tableHolder.tableName.getCipherString() else tableHolder.tableName.getPlainString();
        val stmt = database.createStatement;
        val sql = new StringBuilder("CREATE TABLE '" + tableName + "' ( '");
        
        for (i <- 0 until tableHolder.columnList.size()) {
            if (i != 0) { sql.append("', '") }
            val columnHolder = tableHolder.columnList.get(i);
            val columnName = if (Config.packModeFlag && columnHolder.encryptFlag) columnHolder.columnName.getCipherString() else columnHolder.columnName.getPlainString();
            sql.append(columnName + "' '" + columnHolder.newColumnType);
        }
        sql.append("')");
        logger.info("SQL : " + sql);
        
        stmt.executeUpdate(sql.toString());
        stmt.close();
    }

    def selectAll(tableHolder: TableHolder) = {
        val tableName = if (!Config.packModeFlag && tableHolder.encryptFlag) tableHolder.tableName.getCipherString() else tableHolder.tableName.getPlainString()
        val stmt = database.createStatement;
        val sql = "SELECT * FROM '" + tableName + "'";
        logger.info("SQL : " + sql);
        
        val res = stmt.executeQuery(sql);

        while (res.next) {
            val recordHolder = new RecordHolder();
            for (i <- 0 until tableHolder.columnList.size()) {
                val colHolder = tableHolder.columnList.get(i);
                val bytes: Array[Byte] = colHolder.columnType match {
                    case Constants.REG_EXP_INT(matched) => res.getInt(i + 1).toString().getBytes(Constants.CHAR_ENCODE);
                    case Constants.REG_EXP_REAL(matched) => res.getDouble(i + 1).toString().getBytes(Constants.CHAR_ENCODE);
                    case Constants.REG_EXP_TEXT(matched) => res.getString(i + 1).getBytes(Constants.CHAR_ENCODE);
                    case Constants.REG_EXP_BLOB(matched) => res.getBytes(i + 1);
                    case _ => res.getString(i + 1).getBytes(Constants.CHAR_ENCODE);
                }

                val valueHolder = new CipherHolder(bytes, key, Config.packModeFlag, colHolder.encryptFlag, colHolder.encryptFlag);
                recordHolder.addValue(valueHolder);
                
                logger.debug( "Column : " +  colHolder.columnName.getPlainString());
                logger.debug( "Value(plaintext) : " +  valueHolder.getPlainString());
                logger.debug( "Value(cipher) : " +  valueHolder.getCipherString());
            }
            tableHolder.recordList.add(recordHolder);
            logger.debug( "*************************************************");
        }

        stmt.close();
    }

    def insertAll(tableHolder: TableHolder) = {
        val tableName = if (Config.packModeFlag && tableHolder.encryptFlag) tableHolder.tableName.getCipherString() else tableHolder.tableName.getPlainString()
        val sql = new StringBuilder("INSERT INTO '" + tableName + "' VALUES (?");
        for (i <- 0 until tableHolder.columnList.size()) if (i != 0) sql.append(",?");
        sql.append(")");
        logger.info("SQL : " + sql);
        val recordIte = tableHolder.recordList.iterator();
        while (recordIte.hasNext()) {
            val recordHolder = recordIte.next();
            val pst = database.prepareStatement(sql.toString());
            for (i <- 0 until recordHolder.valueList.size()) {
                val colHolder = tableHolder.columnList.get(i);
                val valueHolder = recordHolder.valueList.get(i);
                if (colHolder.newColumnType == "TEXT") {
                    val value =
                        if (Config.packModeFlag && colHolder.encryptFlag) valueHolder.getCipherString()
                        else valueHolder.getPlainString();
                    pst.setString(i + 1, value);
                } else if (colHolder.newColumnType == "BLOB") {
                    val value =
                        if (Config.packModeFlag && colHolder.encryptFlag) valueHolder.cipher
                        else valueHolder.plainText;
                    pst.setBytes(i + 1, value);
                } else { error("insertAll : insert function only supports TEXT or BLOB type."); }
                
                logger.debug( "Column : " +  colHolder.columnName.getPlainString());
                logger.debug( "Value(plaintext) : " +  valueHolder.getPlainString());
                logger.debug( "Value(cipher) : " +  valueHolder.getCipherString());
                
            }
            pst.executeUpdate();
            pst.close();
            logger.debug( "*************************************************");
        }
    }

    def selectColumn(tableHolder: TableHolder, colHolder: ColumnHolder): ArrayList[CipherHolder] = {
        val tableStr = if (Config.packModeFlag || !tableHolder.encryptFlag) tableHolder.tableName.getPlainString() else tableHolder.tableName.getCipherString();
        val columnStr = if (Config.packModeFlag || !colHolder.encryptFlag) colHolder.columnName.getPlainString() else colHolder.columnName.getCipherString();
        val stmt = database.createStatement();
        val sql = ("SELECT \"" + columnStr + "\" FROM '" + tableStr + "'");
        logger.info("SQL : " + sql);
        val res = stmt.executeQuery(sql);
        val valueList = new ArrayList[CipherHolder]();
        while (res.next) {
            val bytes: Array[Byte] = colHolder.columnType match {
                case Constants.REG_EXP_INT(matched) => res.getInt(1).toString().getBytes(Constants.CHAR_ENCODE);
                case Constants.REG_EXP_REAL(matched) => res.getDouble(1).toString().getBytes(Constants.CHAR_ENCODE);
                case Constants.REG_EXP_TEXT(matched) => res.getString(1).getBytes(Constants.CHAR_ENCODE);
                case Constants.REG_EXP_BLOB(matched) => res.getBytes(1);
                case _ => res.getString(1).getBytes(Constants.CHAR_ENCODE);
            }
            val valueHolder = new CipherHolder(bytes, key, Config.packModeFlag, colHolder.encryptFlag, colHolder.encryptFlag)
            valueList.add(valueHolder);
            logger.debug( "Value(plaintext) : " +  valueHolder.getPlainString());
            logger.debug( "Value(cipher) : " +  valueHolder.getCipherString());
            logger.debug("*************************************************");
        }

        stmt.close();
        return valueList;
    }

    def getFileFromTable(tableHolder: TableHolder, keyCol: ColumnHolder, targetCol: ColumnHolder, fileName: CipherHolder, exceptFlag: Boolean): CipherHolder = {
        val tableStr = if (Config.packModeFlag || !tableHolder.encryptFlag) tableHolder.tableName.getPlainString() else tableHolder.tableName.getCipherString();
        val keyColStr = if (Config.packModeFlag || !keyCol.encryptFlag) keyCol.columnName.getPlainString() else keyCol.columnName.getCipherString();
        val targetColStr = if (Config.packModeFlag || !targetCol.encryptFlag) targetCol.columnName.getPlainString() else targetCol.columnName.getCipherString();
        val fileNameStr = if (Config.packModeFlag || !keyCol.encryptFlag) fileName.getPlainString() else fileName.getCipherString();
        val encryptFileFlag = if (exceptFlag) false else targetCol.encryptFlag

        val stmt = database.createStatement;
        val sql = "SELECT \"" + targetColStr + "\" FROM '" + tableStr + "' WHERE \"" + keyColStr + "\"=\"" + fileNameStr + "\"";
        logger.info("SQL : " + sql);

        val res = stmt.executeQuery(sql);
        val fileBytes = res.getBytes(1);
        stmt.close();
        return new CipherHolder(fileBytes, key, Config.packModeFlag, encryptFileFlag, false);
    }

    def deployFromTables(src: SQLiteMediator) = {
        logger.info("Deploying from database \"" + src.path + "\" to \"" + path + "\"");
        val tableIte = dataHolder.tables.iterator();
        while (tableIte.hasNext()) {
            val tableHolder = tableIte.next();
            val tableName = tableHolder.tableName.getPlainString();
            if( !Config.isReservedTable(tableName) ){
                src.selectAll(tableHolder);
                this.insertAll(tableHolder);
                tableHolder.clearRecords();
            }
        }
    }

    def deployFromFile(src: FileMediator) = {
        val tableName = if (src.encryptFlag == true) src.getTableNameHolder().getCipherString() else src.getTableNameHolder().getPlainString();

        val fileList = src.getFileList();
        for (i <- 0 until fileList.length) {
            val file = fileList(i);
            if (file.getName() != Config.ignoreFile) {
                val fileContent = FileUtils.readFileToByteArray(file);

                val sql = new String("INSERT INTO '" + tableName + "' VALUES (?, ?)");
                logger.info("SQL : " + sql);

                val stmt = database.prepareStatement(sql);
                val nameHolder = new CipherHolder(file.getName().getBytes(Constants.CHAR_ENCODE), key, Config.packModeFlag, encryptFlag, true);
                val contentHolder = new CipherHolder(fileContent, key, Config.packModeFlag, encryptFlag, false);

                if (encryptFlag) {
                    stmt.setString(1, nameHolder.getCipherString());
                    if (src.exceptFlag) stmt.setBytes(2, contentHolder.plainText) else stmt.setBytes(2, contentHolder.cipher);
                } else {
                    stmt.setString(1, nameHolder.getPlainString());
                    stmt.setBytes(2, contentHolder.plainText)
                }
                stmt.executeUpdate();
                stmt.close();
                
                logger.debug( "Name(plaintext) : " +  nameHolder.getPlainString());
                logger.debug( "ContentSize     : " +  contentHolder.plainText.length);
                logger.debug("*************************************************");
            }
        }
    }

    def close() = {
        database.close();
    }
}

class FileMediator(val path: String, val tableName:String, val dataHolder: DatabaseHolder, val target:String) extends LogWriter {
    val key = Config.key;
    val file = new File(path);
    val dirFlag = file.isDirectory();
    val encryptFlag = Config.getEncryptFlag(target);
    val exceptFlag = Config.getExceptFlag(target);

    def getTableNameHolder(): CipherHolder = { return dataHolder.tables.get(0).tableName }
    def getFileList(): List[File] = if (dirFlag == true) file.listFiles().toList else List[File](file)

    def buildTableHolder() = {
        logger.info("Building table for \"" + target + "\"");
        val tableHolder = new TableHolder(tableName, key, Config.packModeFlag, encryptFlag);
        val nameColumnHolder = new ColumnHolder(Constants.TABLE_COLUMN_NAME, "TEXT", key, Config.packModeFlag, encryptFlag);
        tableHolder.addColumn(nameColumnHolder);
        val contentColumnHolder = new ColumnHolder(Constants.TABLE_COLUMN_CONTENT, "BLOB", key, Config.packModeFlag, encryptFlag);
        tableHolder.addColumn(contentColumnHolder);
        dataHolder.addTable(tableHolder);
    }

    def deployFromTable(srcMediator: SQLiteMediator, tableName: String) {
        logger.info("Deploying from table \"" + tableName + "\" to \"" + path + "\"");
        val tableHolder = dataHolder.findTable(tableName);
        val nameColHolder = tableHolder.findColumn(Constants.TABLE_COLUMN_NAME);
        val contentColHolder = tableHolder.findColumn(Constants.TABLE_COLUMN_CONTENT);
        val fileNames = srcMediator.selectColumn(tableHolder, nameColHolder);
        val iteFiles = fileNames.iterator();
        while (iteFiles.hasNext()) {
            val fileName = iteFiles.next();
            val destination = if (dirFlag) path + "/" + fileName.getPlainString() else path;
            val fileHolder = srcMediator.getFileFromTable(tableHolder, nameColHolder, contentColHolder, fileName, exceptFlag);
            val targetFile = new File(destination);
            val writeBytes = if (exceptFlag) fileHolder.cipher else fileHolder.plainText;
            FileUtils.writeByteArrayToFile(targetFile, writeBytes);
        }
    }
}
