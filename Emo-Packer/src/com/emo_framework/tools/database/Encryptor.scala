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
import javax.crypto.spec.SecretKeySpec
import java.security.NoSuchAlgorithmException
import javax.crypto.NoSuchPaddingException
import javax.crypto.IllegalBlockSizeException
import javax.crypto.BadPaddingException
import javax.crypto.Cipher
import javax.crypto.spec.IvParameterSpec
import java.security.AlgorithmParameters
import org.apache.commons.codec.binary.Base64
import javax.crypto.KeyGenerator
import java.security.SecureRandom
import org.apache.commons.io.FileUtils
import java.io.File
import org.apache.commons.codec.binary.Hex

object Encryptor extends LogWriter{

    def encrypt(bytes: Array[Byte], key: Array[Byte]): Array[Byte] = {
        val sKey = new SecretKeySpec(key, Constants.CRYPTO_ALGORITHM);

        val cipher = Cipher.getInstance(Constants.CRYPTO_ENTIRE_ALGORITHM);
        cipher.init(Cipher.ENCRYPT_MODE, sKey);

        // copy initialized IV (random)
        val iv = cipher.getIV();
        val encryptedBytes = cipher.doFinal(bytes);

        // attach IV as a header
        val cipherBytes = new Array[Byte](iv.length + encryptedBytes.length);
        System.arraycopy(iv, 0, cipherBytes, 0, iv.length);
        System.arraycopy(encryptedBytes, 0, cipherBytes, iv.length, encryptedBytes.length);
        return cipherBytes;

    }

    def decrypt(bytes: Array[Byte], key: Array[Byte]): Array[Byte] = {
        val sKey = new SecretKeySpec(key, Constants.CRYPTO_ALGORITHM);

        // get IV from header
        val iv = new Array[Byte](Constants.CRYPTO_BLOCK_SIZE);
        System.arraycopy(bytes, 0, iv, 0, iv.length);
        val paramSpec = new IvParameterSpec(iv);

        // get cipher from the rest
        val encryptedBytes = new Array[Byte](bytes.length - Constants.CRYPTO_BLOCK_SIZE);
        System.arraycopy(bytes, iv.length, encryptedBytes, 0, encryptedBytes.length);

        val cipher = Cipher.getInstance(Constants.CRYPTO_ENTIRE_ALGORITHM);
        cipher.init(Cipher.DECRYPT_MODE, sKey, paramSpec);
        val decryptedBytes = cipher.doFinal(encryptedBytes);
        return decryptedBytes;
    }

    def encryptAndEncode(bytes: Array[Byte], key: Array[Byte]): Array[Byte] = {
        val decryptedBytes = encrypt(bytes, key);
        return Base64.encodeBase64(decryptedBytes);
    }

    def decodeAndDecrypt(bytes: Array[Byte], key: Array[Byte]): Array[Byte] = {
        val decodedBytes = Base64.decodeBase64(bytes);
        return decrypt(decodedBytes, key);
    } 
    
    def generateKey(keyAlgorithm:String, keyLength:Int):Array[Byte] = {
        val generator = KeyGenerator.getInstance(keyAlgorithm);
        val random = SecureRandom.getInstance("SHA1PRNG");
        generator.init(keyLength, random);
        val keyBytes = generator.generateKey().getEncoded();
        val keyOutputFile = new File(Config.keyPath);
        val keyString = new String(Hex.encodeHex(keyBytes));
        logger.info("Generated key : " + keyString);
        FileUtils.writeStringToFile(keyOutputFile, keyString);
        return keyBytes;
    }
}