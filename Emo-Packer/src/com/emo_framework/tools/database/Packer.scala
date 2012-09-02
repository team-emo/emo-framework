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

import java.io.File
import scala.xml._
import scala.util.parsing.combinator._
import java.sql.Connection
import java.sql.DriverManager
import scala.dbc.syntax.Statement._
import scala.dbc.syntax.DataTypeUtil._
import java.net.URI
import scala.dbc.Vendor
import scala.dbc.syntax.Database
import scala.dbc.Database
import org.apache.commons.codec.binary.Hex
import java.util.ArrayList
import org.apache.log4j.Level
import org.apache.log4j.Logger
import org.apache.log4j.PatternLayout
import org.apache.log4j.FileAppender
import org.apache.log4j.BasicConfigurator
import org.apache.log4j.PropertyConfigurator
import java.util.zip.ZipOutputStream
import java.io.FileOutputStream
import java.util.zip.ZipEntry
import java.io.BufferedInputStream
import java.io.FileInputStream
import java.util.zip.ZipInputStream
import java.io.BufferedOutputStream

object Packer extends LogWriter{
    def main(args: Array[String]) = {
        // configure log
        PropertyConfigurator.configure(Constants.LOG_FILE);
        logger.info("Emo-Packer started!");
        logger.info("Important Info : mode = " + Config.packModeString + ", autoGen=" + Config.keyGenString + ", key(XML)=" + Config.keyString )
        
        // execute packing
        try {
            if (Config.packModeFlag) pack() else unpack();
        } catch {
            case e: Exception => logger.error(e);exit();
        }
        
        logger.info("Emo-Packer finished normally!");
    }

    def pack() = {
        val targetFile   =  new File(Config.target);
        if(Config.resetFlag && targetFile.exists()) 
            if(!targetFile.delete()) error("pack : failed to delete target database");
        
        if ( Config.getPackFlag("Tables") == true )  packTables();
        if ( Config.getPackFlag("Config") == true )  packFiles("Config", Constants.TABLE_NAME_CONFIG, false);
        if ( Config.getPackFlag("Scripts") == true ) packFiles("Scripts", Constants.TABLE_NAME_SCRIPTS, true);
        if ( Config.getPackFlag("Sounds") == true )  packFiles("Sounds", Constants.TABLE_NAME_SOUNDS, true);
        if ( Config.getPackFlag("Images") == true )  packFiles("Images", Constants.TABLE_NAME_IMAGES, true);
        if ( Config.getPackFlag("Maps") == true )    packFiles("Maps", Constants.TABLE_NAME_MAPS, true);
        if ( Config.getPackFlag("Models") == true )  packFiles("Models", Constants.TABLE_NAME_MODELS, true);
        
        // zip database file
        if( Config.zipFlag == true) zip(Config.target, Config.target + ".zip");
    }
    
    def unpack() = {
        if ( Config.getPackFlag("Tables") == true )  unpackTables();
        if ( Config.getPackFlag("Config") == true )  unpackFiles("Config", Constants.TABLE_NAME_CONFIG, false);
        if ( Config.getPackFlag("Scripts") == true ) unpackFiles("Scripts", Constants.TABLE_NAME_SCRIPTS, true);
        if ( Config.getPackFlag("Sounds") == true )  unpackFiles("Sounds", Constants.TABLE_NAME_SOUNDS, true);
        if ( Config.getPackFlag("Images") == true )  unpackFiles("Images", Constants.TABLE_NAME_IMAGES, true);
        if ( Config.getPackFlag("Maps") == true )    unpackFiles("Maps", Constants.TABLE_NAME_MAPS, true);
        if ( Config.getPackFlag("Models") == true )  unpackFiles("Models", Constants.TABLE_NAME_MODELS, true);
    }

    def packTables() = packOrUnpackTables(Config.getInputPath("Tables"), Config.target);
    def unpackTables() = {
        if(Config.resetFlag){
            val tableFile = new File(Config.getOutputPath("Tables"));
            if(tableFile.exists() ) if(!tableFile.delete() ) error("unpack : failed to delete target database");
        }
        packOrUnpackTables(Config.target, Config.getOutputPath("Tables"));
    }
    
    def packOrUnpackTables(src:String, dst:String) = {
        val dataHolder = new DatabaseHolder();
        val srcMediator = new SQLiteMediator(src, dataHolder, "Tables");
        val dstMediator = new SQLiteMediator(dst, dataHolder, "Tables");
        srcMediator.buildTableHolders();
        dstMediator.createTables();
        dstMediator.deployFromTables(srcMediator);
        dstMediator.close();
        srcMediator.close();
    }
    
    def unpackFiles(target:String, tableName:String, dirFlag:Boolean){
        val dataHolder = new DatabaseHolder();
        val srcMediator = new SQLiteMediator(Config.target, dataHolder, target);
        val dstMediator = new FileMediator(Config.getOutputPath(target), tableName, dataHolder, target);
        srcMediator.buildTableHolders();
        dstMediator.deployFromTable(srcMediator, tableName);
        srcMediator.close();
    }
    
    def packFiles(target:String, tableName:String, dirFlag:Boolean){
        val dataHolder = new DatabaseHolder();
        val srcMediator = new FileMediator(Config.getInputPath(target), tableName, dataHolder, target);
        val dstMediator = new SQLiteMediator(Config.target, dataHolder, target);
        srcMediator.buildTableHolder();
        dstMediator.createTables();
        dstMediator.deployFromFile(srcMediator);
        dstMediator.close();
    }
    
    def zip(inFileName: String, outFileName: String ) = {
        val zip = new ZipOutputStream(new FileOutputStream(outFileName));
        val fileName = new File(inFileName).getName();
        zip.putNextEntry(new ZipEntry(fileName));
        val in = new BufferedInputStream(new FileInputStream(inFileName));
        var b = in.read();
        while (b > -1) {
            zip.write(b);
            b = in.read();
        }
        in.close();
        zip.closeEntry();
        zip.close();
    }
    
}

object Config {
    val xml          = XML.loadFile(new File("packer.xml"));
    
    // Database
    val target       = (xml \ "Database" \ "@path").toString();
    val zip          = (xml \ "Database" \ "@zip").toString();
    val zipFlag      = zip == "on";
    
    // Pack
    val packModeString = (xml \ "Pack" \ "@mode").toString();
    val packModeFlag = 
        if      ( packModeString == "pack"   ) true;
        else if ( packModeString == "unpack" ) false;
        else error("Config : undefined mode specified on xml file");
    val inputPath  = (xml \ "Pack" \ "@input").toString();
    val outputPath = (xml \ "Pack" \ "@output").toString();
    val resetFlag  = (xml \ "Pack" \ "@reset").toString() == "on";
    val ignoreFile = (xml \ "Pack" \ "@ignore").toString();
    
    // Tables
    def isExceptionTable(tableName:String):Boolean = {
        for (table <- xml \ "Pack" \ "Tables" \ "Encryption" \ "Exception")
            if ( (table \ "@tableName").toString() == tableName) return true;
        return false;
    }
    def isCopyTable(tableName:String):Boolean = {
        for (table <- xml \ "Pack" \ "Tables" \ "Encryption" \ "Exception")
            if ( (table \ "@tableName").toString() == tableName && (table \ "@copy").toString() == "on") return true;
        return false;
    }
    def isReservedTable(tableName:String):Boolean = {
        return tableName match{
            case Constants.TABLE_NAME_CONFIG | Constants.TABLE_NAME_SCRIPTS | Constants.TABLE_NAME_SOUNDS |
                 Constants.TABLE_NAME_IMAGES | Constants.TABLE_NAME_MAPS | Constants.TABLE_NAME_MODELS => true 
            case _ => false
        }
    }
    
    // Files
    def getPackFlag(target:String):Boolean = (xml \ "Pack" \ target \ "@pack").toString() == "on";
    def getInputPath(target:String):String = new String( inputPath +"/" + (xml \ "Pack" \ target \ "@name").toString() );
    def getOutputPath(target:String):String = new String( outputPath + "/" + (xml \ "Pack" \ target \ "@name").toString() );
    def getEncryptFlag(target:String):Boolean = (xml \ "Pack" \ target \ "Encryption" \ "@encrypt").toString == "on";
    def getExceptFlag(target:String):Boolean = (xml \ "Pack" \ target \ "Encryption" \ "@exceptContent").toString == "on";
            
        // Key
    val keyString    = (xml \ "Key" \ "@value").toString();
    val keyGenString = (xml \ "Key" \ "@autoGen").toString();
    val keyGenFlag   = keyGenString== "on";
    val keyPath      = (xml \ "Key" \ "@path").toString();
    val keyEntered   = if( keyString.startsWith("0x") ) Hex.decodeHex(keyString.substring(2).toCharArray());
                       else keyString.getBytes( Constants.CHAR_ENCODE );
    val key: Array[Byte] = if (keyGenFlag && packModeFlag) 
        // generate key if "autoGen" specified on "pack" mode
        Encryptor.generateKey(Constants.CRYPTO_ALGORITHM, Constants.CRYPTO_KEY_LENGTH) else keyEntered;

}

trait LogWriter {
    val loggerName = this.getClass.getName;
    lazy val logger = Logger.getLogger(loggerName);
}
