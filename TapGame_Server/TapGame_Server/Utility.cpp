#include "Utility.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>

#define SALT_SIZE 16       // ��Ʈ ũ�� (16����Ʈ)
#define HASH_SIZE 32       // �ؽ� ũ�� (32����Ʈ, SHA-256)
#define ITERATIONS 100000  // �ݺ� Ƚ�� (���� ��ȭ)

std::string Utility::bytesToHex(const unsigned char* bytes, size_t length)
{
    std::ostringstream oss;

    for (size_t i = 0; i < length; i++)    
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)bytes[i];    

    return oss.str();
}

void Utility::hexToBytes(const std::string& hex, unsigned char* bytes, size_t length)
{
    for (size_t i = 0; i < length; ++i) 
    {
        std::string byteString = hex.substr(i * 2, 2);
        bytes[i] = (unsigned char)strtol(byteString.c_str(), nullptr, 16);
    }
}

std::string Utility::hashPassword(const std::string& password)
{
    unsigned char salt[SALT_SIZE];
    unsigned char hash[HASH_SIZE];

    // ������ ��Ʈ ����
    if (!RAND_bytes(salt, SALT_SIZE))
    {
        throw std::runtime_error("��Ʈ ���� ����");
    }

    // PBKDF2 �ؽ� ����
    if (!PKCS5_PBKDF2_HMAC(
        password.c_str(),       //���� �н�����
        password.size(),        //���� �н����� ������
        salt,                   //��Ʈ ���� ����
        SALT_SIZE,              //��Ʈ ������
        ITERATIONS,             //�ݺ� Ƚ�� (Ƚ���� �������� ������ �ö󰣴�)
        EVP_sha512(),           //��� �˰���
        HASH_SIZE,              //�ؽ� ������ 
        hash                    //�ؽ� ���� ����
    ))
    {
        throw std::runtime_error("��й�ȣ �ؽ� ����");
    }

    // �ؽÿ� ��Ʈ�� 16���� ���ڿ��� ��ȯ
    return Utility::bytesToHex(salt, SALT_SIZE) + Utility::bytesToHex(hash, HASH_SIZE);
}

bool Utility::verifyPassword(const std::string password, const std::string storedHash)
{
    //DB���� �ؽ��� �� ã�� ��� ��� ��ȯ
    if (storedHash == "")
        return false;

    unsigned char salt[SALT_SIZE];
    unsigned char hash[HASH_SIZE];
    unsigned char newHash[HASH_SIZE];

    // ����� �ؽÿ��� ��Ʈ�� �ؽ� �� ����
    Utility::hexToBytes(storedHash.substr(0, SALT_SIZE * 2), salt, SALT_SIZE);
    Utility::hexToBytes(storedHash.substr(SALT_SIZE * 2), hash, HASH_SIZE);

    // �Էµ� ��й�ȣ�� ���� ������� �ؽ�
    if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.size(), salt, SALT_SIZE, ITERATIONS, EVP_sha512(), HASH_SIZE, newHash)) {
        throw std::runtime_error("��й�ȣ �ؽ� ����");
    }

    // ���� �ؽÿ� ���ο� �ؽ� ��
    return std::memcmp(hash, newHash, HASH_SIZE) == 0;
}

std::string Utility::fillZero(std::string str, int width)
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(width) << str;
    str = oss.str();
    return str;
}
