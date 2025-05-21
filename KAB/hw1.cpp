#ifndef __PROGTEST__
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

#endif /* __PROGTEST__ */
string to_hex(const unsigned char * bin, size_t length) {
    string hex;
    hex.reserve(length * 2);
    for (size_t i = 0; i < length; ++i) {
        char buf[3];
        sprintf(buf, "%02x", bin[i]);
        hex.append(buf);
    }
    return hex;
}
bool zero_bytes(const unsigned char * hash, int hash_length, int bits){
    int zero_o=bits/8;
    int zero_dop=bits%8;

    for(int i=0; i<zero_o;++i){
        if(i>=hash_length || hash[i]!=0x00){
            return false;
        }
    }
    if(zero_dop>0 && zero_o<hash_length){
        unsigned char mask=0xFF<<(8-zero_dop);
        if((hash[zero_o] & mask)!=0){
            return false;
        }
    }
    return true;
}

int findHash (int numberZeroBits, string & outputMessage, string & outputHash) {
    if(numberZeroBits < 0 || numberZeroBits>512){
        return 0;
    }
    unsigned char ran_message[64];
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length;
    EVP_MD_CTX *ctx;

    while (true)
    {
        if (!RAND_bytes(ran_message, sizeof(ran_message))) {
            return 0;
        }
        ctx = EVP_MD_CTX_new();
        if (ctx == nullptr){
            return 0;
        }
        if (!EVP_DigestInit_ex(ctx, EVP_sha512(), nullptr)){
            EVP_MD_CTX_free(ctx);
            return 0;
        }
        if (!EVP_DigestUpdate(ctx, ran_message, sizeof(ran_message))){
            EVP_MD_CTX_free(ctx);
            return 0;
        }
        if (!EVP_DigestFinal_ex(ctx, hash, &hash_length)){
            EVP_MD_CTX_free(ctx);
            return 0;
        }
        if(zero_bytes(hash,hash_length,numberZeroBits)){
            outputMessage = to_hex(ran_message, sizeof(ran_message));
            outputHash=to_hex(hash,hash_length);
            EVP_MD_CTX_free(ctx);
            return 1;
        }
        else{
            EVP_MD_CTX_free(ctx);
        }
    }
}

int findHashEx (int numberZeroBits, string & outputMessage, string & outputHash, string_view hashType) {
    /* TODO or use dummy implementation */
    return 1;
}

#ifndef __PROGTEST__

int checkHash(int bits, const string & hash){
    if(bits < 0 || bits > 512){
        return 0;
    }
    unordered_map<char, string> hex_to_bin = {
            {'0', "0000"}, {'1', "0001"},
            {'2', "0010"}, {'3', "0011"},
            {'4', "0100"}, {'5', "0101"},
            {'6', "0110"}, {'7', "0111"},
            {'8', "1000"}, {'9', "1001"},
            {'a', "1010"}, {'b', "1011"},
            {'c', "1100"}, {'d', "1101"},
            {'e', "1110"}, {'f', "1111"}
    };

    string binary;
    for(size_t i = 0; i < hash.length(); ++i){
        binary += hex_to_bin[hash[i]];
    }

    for(int i = 0; i < bits; ++i){
        if (binary[i] != '0') {
            return 0;
        }
    }
    return 1;
}


int main (void) {
    string hash, message;
    assert(findHash(0, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(0, hash));
    message.clear();
    hash.clear();
    assert(findHash(1, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(1, hash));
    message.clear();
    hash.clear();
    assert(findHash(2, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(2, hash));
    message.clear();
    hash.clear();
    assert(findHash(3, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(3, hash));
    message.clear();
    hash.clear();
    assert(findHash(-1, message, hash) == 0);
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
