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
#if __cplusplus
extern "C" {
#endif

void LOGI(const char* msg);
void LOGE(const char* msg);
void LOGW(const char* msg);

#include <math.h>

bool isPowerOfTwo(int x) {
	return (x != 0) && ((x & (x - 1)) == 0);
}

int nextPowerOfTwo(int minimum) {
	if(isPowerOfTwo(minimum)) {
		return minimum;
	}
	int i = 0;
	while(true) {
		i++;
		if(pow(2, i) >= minimum) {
			return (int)pow(2, i);
		}
	}
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

#if __cplusplus
}   // Extern C

extern "C++" {

#include "Constants.h"
#include "Util.h"
#include "aes.h"
#include "aescpp.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>
using namespace std;

vector <string> split( string s, string c )
{

    vector<string> ret;
    for( unsigned int i=0, n; i <= s.length(); i=n+1 ){
        n = s.find_first_of( c, i );
        if( n == string::npos ) n = s.length();
        string tmp = s.substr( i, n-i );
        ret.push_back(tmp);
    }
    return ret;
}
    
static char encoding_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/=";

char* base64Encode(const unsigned char *src, int size){

    char* dst = (char *) calloc(( size * 2) + 1, sizeof(char) );

    // split every 3 byte data into 6bit blocks
    for( int i = 0, j = 0 ; i < size ; i += 3,j += 4){
        unsigned char buf1, buf2, buf3;
        unsigned char ind1, ind2, ind3, ind4;

        // retrieving char code
        buf1 = (i < size - 0) ? *(src + i + 0) : '\0';
        buf2 = (i < size - 1) ? *(src + i + 1) : '\0';
        buf3 = (i < size - 2) ? *(src + i + 2) : '\0';

        // 3 characters into 4 characters
        ind1 =                      (buf1 >> 2);
        ind2 = (buf1 & 0x03) << 4 | (buf2 >> 4); // 0x3f == 111111 (bin)
        ind3 = (buf2 & 0x0f) << 2 | (buf3 >> 6); // 0x0f == 1111   (bin)
        ind4 = (buf3 & 0x3f);

        // encode input according to encoding_table
        *(dst + j + 0) = encoding_table[ind1];
        *(dst + j + 1) = encoding_table[ind2];
        *(dst + j + 2) = ( i < size - 1 ) ? encoding_table[ind3] : '=';// turn fraction into '='
        *(dst + j + 3) = ( i < size - 2 ) ? encoding_table[ind4] : '=';
    }

    return dst;
}

unsigned char* base64Decode(const char *src, int* size ){

    *size = 0;
    int strSize = strlen(src);
    unsigned char* dst = (unsigned char *) calloc( strSize + 3, sizeof(char) );

    for( int i = 0, j = 0 ; i < strSize ; i += 4 , j += 3){
        unsigned char ind1, ind2, ind3, ind4;

        // retrieve char code
        ind1 = *(src + i + 0) == '=' ? 0 : strchr(encoding_table, *(src + i + 0)) - encoding_table;
        ind2 = *(src + i + 1) == '=' ? 0 : strchr(encoding_table, *(src + i + 1)) - encoding_table;
        ind3 = *(src + i + 2) == '=' ? 0 : strchr(encoding_table, *(src + i + 2)) - encoding_table;
        ind4 = *(src + i + 3) == '=' ? 0 : strchr(encoding_table, *(src + i + 3)) - encoding_table;

        // deconde into bytes and count if the byte is in range
        *(dst + j + 0) = (unsigned char)( (ind1 & 0x3f) << 2 | (ind2 & 0x30) >> 4 );
        if( *(dst + j + 0) != 0 || ( (*(src + i + 0) != '=') && *(src + i + 1) != '=' ) ) (*size)++;

        *(dst + j + 1) = (unsigned char)( (ind2 & 0x0f) << 4 | (ind3 & 0x3c) >> 2 );
        if( *(dst + j + 1) != 0 || ( (*(src + i + 1) != '=') && *(src + i + 2) != '=' ) ) (*size)++;

        *(dst + j + 2) = (unsigned char)( (ind3 & 0x03) << 6 | (ind4 & 0x3f) >> 0 );
        if( *(dst + j + 2) != 0 || ( (*(src + i + 2) != '=') && *(src + i + 3) != '=' ) ) (*size)++;

    }

    return dst;
}

unsigned char* emoEncrypt(const unsigned char* key, const unsigned char* src, int size, int* cipherSize){

    // initialize encryptor
    AESencrypt * encryptor = new AESencrypt(key);

    // prepare initialisation vector
    unsigned char iv [AES_BLOCK_SIZE] = {0};
    unsigned char ivClone [AES_BLOCK_SIZE] = {0};
    for( int i = 0 ; i < AES_BLOCK_SIZE ; i++ ) {
        iv[i] = rand() & 0xFF;
        ivClone[i] = iv[i];
    }

    // check padding size
    int padding = (size > AES_BLOCK_SIZE) ? AES_BLOCK_SIZE - ( size % AES_BLOCK_SIZE ) : AES_BLOCK_SIZE - size;
    if(padding == 0) padding = AES_BLOCK_SIZE; // apply PKCS#5 padding rule
    int targetDataSize = size + padding;

    // prepare buffers
    unsigned char * targetData = (unsigned char *) calloc( targetDataSize + 1, sizeof(char));
    unsigned char * encryptedData = (unsigned char *) calloc( targetDataSize + 1, sizeof(char));
    memcpy(targetData, src, size);

    // pad with number of padded bytes
    for(int i = 0, offset = targetDataSize-1 ; i < padding ; i++, offset--) targetData[offset] = padding;

    // encrypt with CBC mode
    AES_RETURN aesResult = encryptor->cbc_encrypt(targetData, encryptedData, targetDataSize, iv);
    if( aesResult != EXIT_SUCCESS ){
        LOGE("encryptCbcMode: AES(CBC mode) encryption failed");
        free(targetData);
        free(encryptedData);
        return NULL;
    }

    // complete encryption by attaching IV
    unsigned char * cipher = 0;
    *cipherSize = targetDataSize + AES_BLOCK_SIZE ;
    cipher = (unsigned char *) calloc( *cipherSize + 1, sizeof(char));

    // attach IV to the head
    memcpy(cipher, ivClone, AES_BLOCK_SIZE);
    memcpy(&cipher[AES_BLOCK_SIZE], encryptedData, targetDataSize);
    free(encryptedData);

    return cipher;
}

unsigned char* emoDecrypt(const unsigned char* key, const unsigned char* src, int size, int* plainSize) {

    // Initialize decryptor
    AESdecrypt * decryptor = new AESdecrypt(key);

    // prepare initialisation vector
    unsigned char iv [AES_BLOCK_SIZE] = {0};
    memcpy(iv, src, AES_BLOCK_SIZE);

    // memory allocation for decrypted bytes
    unsigned char * plainBytes = 0;
    *plainSize = size - AES_BLOCK_SIZE; // subtract block size for IV
    plainBytes = (unsigned char *) calloc( *plainSize, sizeof(char) );

    // decrypt with CBC mode
    AES_RETURN aesResult = decryptor->cbc_decrypt(&src[AES_BLOCK_SIZE], plainBytes, *plainSize , iv);
    if( aesResult != EXIT_SUCCESS ){
         LOGE("decryptCbcMode: AES(CBC mode) decryption failed");
         free(plainBytes);
         return NULL;
     }

    // convert padded bytes to NULL
    unsigned char padding = plainBytes[*plainSize - 1];
    for(int i = 0 ; i < (int)padding ; i++ ){
        plainBytes[*plainSize -1 - i] = NULL;
    }
    *plainSize -= padding;

    return plainBytes;

}

string encryptString(const string& src){

    // convert string to a single space if null string specified
    string targetString = src;
    if( src.empty() == true){
        targetString = string(" ");
    }

    // get bytes from string
    int size = targetString.size();
    unsigned char *  bytes = (unsigned char *) targetString.data();

    // retrieve the common (private) key
    const unsigned char * key = (unsigned char *)AES_PRIVATE_KEY;

    // encrypt bytes
    int cipherSize = 0;
    unsigned char * encryptedBytes = emoEncrypt(key, bytes, size, &cipherSize);

    // base64 encoding
    char * encodedChars = base64Encode(encryptedBytes, cipherSize) ;
    string dst = string(encodedChars);

    return dst;
}

string decryptString(const string& src){

    // base64 decoding
    int size = 0;
    unsigned char * decodedBytes = base64Decode( src.c_str(), &size );

    // retrieve common (private) key
    const unsigned char * key = (unsigned char *)AES_PRIVATE_KEY;

    // decrypt bytes
    int plainSize = 0;
    unsigned char * decryptedBytes = emoDecrypt(key, decodedBytes, size, &plainSize);
    string dst = string( (char*) decryptedBytes);

    return dst;
}

} // Extern C++
#endif
