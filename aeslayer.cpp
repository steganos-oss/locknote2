
#include <iostream>

// CryptoPP includes
#include "sha.h"
#include "pwdbased.h"
#include "scrypt.h"
#include "aes.h"
#include "modes.h"
#include "hmac.h"

#include "aeslayer.h"

#include "hex.h"
#include "files.h"

using namespace std;

NAMESPACE_BEGIN(CryptoPP)

// encryption function:
// use PKCS#7 padding to align to block size for AES CBC mode
// derive a key from a salted passphrase using PBKDF2 or SCrypt
// encrypt-then-mac using HMAC-SHA256
// MAC is generated over ciphertext and IV
unsigned int AESLayer::Encrypt(RandomNumberGenerator& rng, ConstByteArrayParameter const& passphrase, byte* output, const std::string& plaintext)
{
	// determine number of bytes for PKCS#7 padding
	unsigned int padding_length = AES::BLOCKSIZE - (plaintext.size() % AES::BLOCKSIZE);
	unsigned int padded_size = plaintext.size() + padding_length;

	// pad plain text with (padding_length * <value of padding_length>)
	//std::unique_ptr<char>out_buffer = std::make_unique<char>(padded_size + 1);
	byte* out_buffer = new byte[padded_size];
	memset(out_buffer, 0, padded_size);
	memcpy_s(out_buffer, padded_size, plaintext.c_str(), plaintext.size());

	// write padding bytes after end of the payload
	memset(out_buffer + plaintext.size(), padding_length, padding_length);
	
	// establish ciphertext segment locations
	byte* salt = output;
	byte* payload = salt + AESLayer::SALT_SIZE;
	byte* iv_seed = payload + padded_size;
	byte* digest = iv_seed + AESLayer::IV_SEED_SIZE;
	unsigned int output_length = (digest + HMAC<SHA256>::DIGESTSIZE) - output;

	// generate a random salt for password derivation
	rng.GenerateBlock(salt, AESLayer::SALT_SIZE);

	// generate IV seed
	rng.GenerateBlock(iv_seed, AESLayer::IV_SEED_SIZE);

#ifdef _KEY_DERIVATION_USE_SCRYPT
	// derive a key from the passphrase using SCrypt
	Scrypt scrypt;
	SecByteBlock key(SHA256::DIGESTSIZE);
	scrypt.DeriveKey(key.begin(), key.size(), passphrase.begin(), passphrase.size(), salt, AESLayer::SALT_SIZE, AESLayer::DERIVATION_COST, 8, 5);
#else // use PBKDF2
	// derive a key from the passphrase using PKBDF2 with SHA-256
	PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
	const byte purpose_unused = 0;
	SecByteBlock key(SHA256::DIGESTSIZE);
	pbkdf.DeriveKey(key.begin(), key.size(), purpose_unused, passphrase.begin(), passphrase.size(), salt, AESLayer::SALT_SIZE, AESLayer::KEY_ITERATIONS, 0.0);
#endif

	// generate initialization vector from seed
	SecByteBlock iv(AESLayer::IV_SIZE);
#ifdef _KEY_DERIVATION_USE_SCRYPT
	scrypt.DeriveKey(iv.begin(), iv.size(), passphrase.begin(), passphrase.size(), iv_seed, AESLayer::IV_SEED_SIZE, 2, 8, 5);
#else
	pbkdf.DeriveKey(iv.begin(), iv.size(), purpose_unused, passphrase.begin(), passphrase.size(), ivSeed, AESLayer::IV_SEED_SIZE, AESLayer::KEY_ITERATIONS, 0.0);
#endif

	// instantiate encryptor using derived key and iv
	CBC_Mode<AES>::Encryption encryptor(key.begin(), key.size(), iv.begin());

	// encrypt padding and payload
	encryptor.ProcessString(payload, (const byte*)out_buffer, padded_size);

	// calculate MAC over padded plaintext and iv seed
	HMAC<SHA256>(key.begin(), key.size()).CalculateDigest(digest, payload, digest - payload);

	// clear plain text from out buffer and free
	memset(out_buffer, 0, padded_size);
	delete[] out_buffer;
	out_buffer = nullptr;

	return output_length;
}

// decryption function
// derive a key from a salted passphrase using PBKDF2 or SCrypt
// check MAC tag by generating a HMAC-SHA256 over ciphertext and IV
// then decrypt and remove padding
DecodingResult AESLayer::Decrypt(ConstByteArrayParameter const& passphrase, byte* output, ConstByteArrayParameter const& input)
{
	if (input.size() < MINIMUM_CIPHERTEXT_LENGTH)
	{
		return DecodingResult();
	}

	// establish locations of salt, IV seed and MAC
	byte const* salt = input.begin();
	byte const* payload = salt + AESLayer::SALT_SIZE;
	byte const* digest = input.end() - HMAC<SHA256>::DIGESTSIZE;
	byte const* iv_seed = digest - AESLayer::IV_SEED_SIZE;

#ifdef _KEY_DERIVATION_USE_SCRYPT
	// derive a key from the passphrase using SCrypt
	Scrypt scrypt;
	SecByteBlock key(SHA256::DIGESTSIZE);
	scrypt.DeriveKey(key.begin(), key.size(), passphrase.begin(), passphrase.size(), salt, AESLayer::SALT_SIZE, AESLayer::DERIVATION_COST, 8, 5);
#else // use PBKDF2
	// derive a key from the passphrase using PKBDF2 with SHA-256
	PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
	const byte purpose_unused = 0;
	SecByteBlock key(SHA256::DIGESTSIZE);
	pbkdf.DeriveKey(key.begin(), key.size(), purpose_unused, passphrase.begin(), passphrase.size(), salt, AESLayer::SALT_SIZE, AESLayer::KEY_ITERATIONS, 0.0);
#endif

	// verify MAC
	byte check_digest[HMAC<SHA256>::DIGESTSIZE];
	HMAC<SHA256>(key.begin(), key.size()).CalculateDigest(check_digest, payload, digest - payload);
	if (memcmp(digest, check_digest, sizeof check_digest) != 0)
	{
		return DecodingResult();
	}

	// generate initialization vector from seed
	SecByteBlock iv(AESLayer::IV_SIZE);
#ifdef _KEY_DERIVATION_USE_SCRYPT
	scrypt.DeriveKey(iv.begin(), iv.size(), passphrase.begin(), passphrase.size(), iv_seed, AESLayer::IV_SEED_SIZE, 2, 8, 5);
#else
	pbkdf.DeriveKey(iv.begin(), iv.size(), purpose_unused, passphrase.begin(), passphrase.size(), ivSeed, AESLayer::IV_SEED_SIZE, AESLayer::KEY_ITERATIONS, 0.0);
#endif

	// instantiate decryptor using derived key and iv
	CBC_Mode<AES>::Decryption decryptor(key.begin(), key.size(), iv.begin());

	// determine padded length
	unsigned int payload_len = iv_seed - payload;

	if (payload_len)
	{
		// decrypt payload
		decryptor.ProcessString(output, payload, payload_len);

		// see how many padding bytes we have, truncate to real payload length
		byte pad_byte = output[payload_len - 1];
		payload_len = payload_len - pad_byte;
	}

	return DecodingResult(payload_len);
}

NAMESPACE_END
