#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/pem.h>


using namespace std;

#endif /* __PROGTEST__ */


bool seal( string_view inFile, string_view outFile, string_view publicKeyFile, string_view symmetricCipher )
{
    OpenSSL_add_all_algorithms();
    if(inFile.empty() || outFile.empty() || publicKeyFile.empty() || symmetricCipher.empty()){
        remove(outFile.data());
        return false;
    }

    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> context(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if(!context){
        remove(outFile.data());
        return false;
    }

    const EVP_CIPHER *cipher = EVP_get_cipherbyname(symmetricCipher.data());
    if(!cipher){
        remove(outFile.data());
        return false;
    }
    int nid = EVP_CIPHER_nid(cipher);

    FILE *pubKeyFile = fopen(publicKeyFile.data(), "r");
    if(!pubKeyFile){
        remove(outFile.data());
        return false;
    }

    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pubKey(PEM_read_PUBKEY(pubKeyFile, nullptr, nullptr, nullptr), EVP_PKEY_free);
    fclose(pubKeyFile);
    if(!pubKey){
        remove(outFile.data());
        return false;
    }

    std::vector<unsigned char> encryptedKey(EVP_PKEY_size(pubKey.get()));
    int encryptedKeyLength = 0;
    std::vector<unsigned char> iv(EVP_CIPHER_iv_length(cipher));

    unsigned char *ek = encryptedKey.data();
    unsigned char *iv_ptr = iv.data();
    EVP_PKEY *pub_key_ptr = pubKey.get();

    if(!EVP_SealInit(context.get(), cipher, &ek, &encryptedKeyLength, iv_ptr, &pub_key_ptr, 1)){
        remove(outFile.data());
        return false;
    }

    std::ifstream inFileStream(inFile.data(), std::ios::binary);
    std::ofstream outFileStream(outFile.data(), std::ios::binary);
    if(!inFileStream.is_open() || !outFileStream.is_open()){
        remove(outFile.data());
        return false;
    }

    outFileStream.write(reinterpret_cast<char*>(&nid), sizeof(nid));
    if(outFileStream.bad()){
        remove(outFile.data());
        return false;
    }
    outFileStream.write(reinterpret_cast<char*>(&encryptedKeyLength), sizeof(encryptedKeyLength));
    if(outFileStream.bad()){
        remove(outFile.data());
        return false;
    }
    outFileStream.write(reinterpret_cast<char*>(ek), encryptedKeyLength);
    if(outFileStream.bad()){
        remove(outFile.data());
        return false;
    }

    outFileStream.write(reinterpret_cast<char*>(iv_ptr), iv.size());
    if(outFileStream.bad()){
        remove(outFile.data());
        return false;
    }


    std::vector<unsigned char> buffer(4096);
    std::vector<unsigned char> buffer_to(4096 + EVP_MAX_BLOCK_LENGTH);
    int outLen=0;

    for (;;) {
        inFileStream.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        int bytesReadInp = inFileStream.gcount();
        if(bytesReadInp > 0){
            if(!EVP_SealUpdate(context.get(), buffer_to.data(), &outLen, buffer.data(), bytesReadInp)){
                remove(outFile.data());
                return false;
            }
            outFileStream.write(reinterpret_cast<char*>(buffer_to.data()), outLen);
            if(outFileStream.bad()){
                remove(outFile.data());
                return false;
            }
        }
        if (inFileStream.eof()) {
            break;
        } else if (inFileStream.bad()){
            remove(outFile.data());
            return false;
        }
    }


    if (!EVP_SealFinal(context.get(), buffer_to.data(), &outLen)){
        remove(outFile.data());
        return false;
    }
    outFileStream.write(reinterpret_cast<char*>(buffer_to.data()), outLen);
    if(outFileStream.bad()){
        remove(outFile.data());
        return false;
    }

    return true;
}




bool open(std::string_view inFile, std::string_view outFile, std::string_view privateKeyFile) {
    if (inFile.empty() || outFile.empty() || privateKeyFile.empty()){
        remove(outFile.data());
        return false;
    }

    OpenSSL_add_all_algorithms();
    std::ifstream encryptedFile(inFile.data(), std::ios::binary);
    std::ofstream decryptedFile(outFile.data(), std::ios::binary);

    if(!encryptedFile.is_open() || !decryptedFile.is_open()){
        remove(outFile.data());
        return false;
    }

    FILE* privKeyFile = fopen(privateKeyFile.data(), "r");
    if(!privKeyFile){
        remove(outFile.data());
        return false;
    }
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> privKey(PEM_read_PrivateKey(privKeyFile, nullptr, nullptr, nullptr), EVP_PKEY_free);
    fclose(privKeyFile);

    if(!privKey){
        remove(outFile.data());
        return false;
    }

    int nid, encryptedKeyLength;
    if(!encryptedFile.read(reinterpret_cast<char*>(&nid), sizeof(nid))){
        remove(outFile.data());
        return false;
    }
    if(encryptedFile.gcount()!= 4){
        remove(outFile.data());
        return false;
    }
    if(encryptedFile.bad()){
        remove(outFile.data());
        return false;
    }



    if(!encryptedFile.read(reinterpret_cast<char*>(&encryptedKeyLength), sizeof(encryptedKeyLength))){
        remove(outFile.data());
        return false;
    }
    if(encryptedFile.gcount() != 4){
        remove(outFile.data());
        return false;
    }
    if(encryptedFile.bad()){
        remove(outFile.data());
        return false;
    }

    const EVP_CIPHER* cipher = EVP_get_cipherbynid(nid);
    if (!cipher){
        remove(outFile.data());
        return false;
    }

    size_t iv_length = EVP_CIPHER_iv_length(cipher);

    std::vector<unsigned char> encryptedKey(encryptedKeyLength);
    if(encryptedFile.read(reinterpret_cast<char*>(encryptedKey.data()), encryptedKeyLength).gcount() != encryptedKeyLength){
        remove(outFile.data());
        return false;
    }
    if (encryptedFile.bad()){
        remove(outFile.data());
        return false;
    }


    std::vector<unsigned char> iv(iv_length);
    if(encryptedFile.read(reinterpret_cast<char*>(iv.data()), iv_length).gcount() != static_cast<std::streamsize>(iv_length)){
        remove(outFile.data());
        return false;
    }
    if(encryptedFile.bad()){
        remove(outFile.data());
        return false;
    }



    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx(EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free);
    if(!EVP_OpenInit(ctx.get(), cipher, encryptedKey.data(), encryptedKeyLength, iv.data(), privKey.get())){
        remove(outFile.data());
        return false;
    }

    std::vector<unsigned char> buffer(4096);
    std::vector<unsigned char> buffer_to(4096 + EVP_MAX_BLOCK_LENGTH);
    int outLen = 0;

    for(;;){
        encryptedFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        int bytesRead = encryptedFile.gcount();
        if(bytesRead>0){
            if (!EVP_OpenUpdate(ctx.get(), buffer_to.data(), &outLen, buffer.data(), bytesRead)){
                remove(outFile.data());
                return false;
            }
            decryptedFile.write(reinterpret_cast<char*>(buffer_to.data()), outLen);
            if(decryptedFile.bad()){
                remove(outFile.data());
                return false;
            }

        }

        if(encryptedFile.eof()){
            break;
        } else if (encryptedFile.bad()){
            remove(outFile.data());
            return false;
        }
    }

    if(!EVP_OpenFinal(ctx.get(), buffer_to.data(), &outLen)){
        remove(outFile.data());
        return false;
    }

    decryptedFile.write(reinterpret_cast<char*>(buffer_to.data()), outLen);
    if(decryptedFile.bad()){
        remove(outFile.data());
        return false;
    }


    return true;

}



#ifndef __PROGTEST__

int main ( void )
{

    assert( seal("fileToEncrypt.txt", "sealed.bin", "PublicKey.pem", "aes-128-cbc") );
    assert( open("sealed.bin", "openedFileToEncrypt.txt", "PrivateKey.pem") );

    assert( open("sealed_sample.bin", "opened_sample.txt", "PrivateKey.pem") );

    return 0;
}

#endif /* __PROGTEST__ */

