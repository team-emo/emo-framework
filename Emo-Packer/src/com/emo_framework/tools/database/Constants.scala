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

object Constants {
    // encryption algorithm
    val CRYPTO_ALGORITHM = "AES";
    val CRYPTO_ENTIRE_ALGORITHM = "AES/CBC/PKCS5Padding";
    val CRYPTO_KEY_LENGTH = 128;
    val CRYPTO_BLOCK_SIZE = 16;
    val CHAR_ENCODE = "US-ASCII";
    
    // tables for storing files
    val TABLE_NAME_CONFIG  = "CONFIG";
    val TABLE_NAME_SCRIPTS = "SCRIPTS";
    val TABLE_NAME_SOUNDS  = "SOUNDS";
    val TABLE_NAME_IMAGES  = "IMAGES";
    val TABLE_NAME_MAPS    = "MAPS";
    val TABLE_NAME_MODELS  = "MODELS";
    
    // column names for new tables
    val TABLE_COLUMN_NAME    = "NAME";
    val TABLE_COLUMN_CONTENT = "CONTENT";
    
    // regular expressions for detecting types
    def REG_EXP_INT  = "(?i)(^int.*)".r;
    def REG_EXP_REAL = "(?i)(^real.*|^doub.*|^floa.*)".r;
    def REG_EXP_TEXT = "(?i)(^tex.*|^char.*|^clob.*)".r;
    def REG_EXP_BLOB = "(?i)(^blob.*|^none.*)".r;
    
    // log4j config file
    val LOG_FILE = "log4j.properties"
}