#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                       RSA  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string rsaEncrypt(const std::string& plaintext, RSA* rsaPublicKey) {
    int rsaLen = RSA_size(rsaPublicKey);
    unsigned char* ciphertext = new unsigned char[rsaLen];
    int result = RSA_public_encrypt(plaintext.size(), reinterpret_cast<const unsigned char*>(plaintext.c_str()), ciphertext, rsaPublicKey, RSA_PKCS1_PADDING);
    if (result == -1) {
        // Error occurred
        std::cerr << "RSA encryption failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        delete[] ciphertext;
        return "";
    }
    std::string encryptedText(reinterpret_cast<char*>(ciphertext), result);
    delete[] ciphertext;
    return encryptedText;
}

int ahmain() {
    // Load public key from a file (you can load it from any source)
    FILE* publicKeyFile = fopen("public_key.pem", "rb");
    if (!publicKeyFile) {
        std::cerr << "Error loading public key file." << std::endl;
        return 1;
    }

    RSA* rsaPublicKey = PEM_read_RSA_PUBKEY(publicKeyFile, nullptr, nullptr, nullptr);
    fclose(publicKeyFile);

    if (!rsaPublicKey) {
        std::cerr << "Error loading RSA public key." << std::endl;
        return 1;
    }

    // Plaintext to be encrypted
    std::string plaintext = "Hello, World!";

    // Encrypt the plaintext
    std::string encryptedText = rsaEncrypt(plaintext, rsaPublicKey);

    if (!encryptedText.empty()) {
        std::cout << "Encrypted text: " << encryptedText << std::endl;
    }

    // Free the public key
    RSA_free(rsaPublicKey);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                       B64  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

std::string base64_encode(const unsigned char* input, int length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    return std::string(bufferPtr->data, bufferPtr->length);
}

int mgaain() {
    // Your RSA encryption code here
    RSA *rsa = nullptr; // Initialize your RSA object
    unsigned char encryptedData[256]; // Assuming RSA encryption produces 256 bytes

    // Perform RSA encryption and store the result in encryptedData

    // Base64 encode the encrypted data
    std::string base64_encoded = base64_encode(encryptedData, 256);

    // Output the base64 encoded string
    std::cout << "Base64 Encoded RSA encrypted string: " << base64_encoded << std::endl;

    return 0;
}
