#ifndef _AESLAYER_H_
#define _AESLAYER_H_

#include "algparam.h"

NAMESPACE_BEGIN(CryptoPP)

// AES Encryption Layer with confidentiality and authentication
// ==========================================================================
// This class uses CryptoPP's AES encryption implementation in CBC mode,
// with PKCS#7 padding to align to AES block size for CBC and a HMAC-SHA256
// over the ciphertext and initialization vector for message authentication.
// In addition, the key/password is derived using SCrypt (or optionally
// PBKDF2-SHA256) from the given passphrase, which is salted before.
//
// The class is losely based on Denis Bider's AESPHM, with notable changes
// being standard PKCS#7 padding at the end of the plaintext, more resilient
// password derivation, use of CBC instead of CFB, and a modern hashing algo 
// for HMAC generation.

#define _KEY_DERIVATION_USE_SCRYPT // use SCrypt instead of the older PBKDF2 for password derivation

class AESLayer
{
public:
	static constexpr unsigned int MAX_PADDING_BYTES = AES::BLOCKSIZE;
	static constexpr unsigned int SALT_SIZE = 16;
	static constexpr unsigned int IV_SIZE = AES::BLOCKSIZE;
	static constexpr unsigned int IV_SEED_SIZE = IV_SIZE;
#ifdef _KEY_DERIVATION_USE_SCRYPT
	#ifdef _DEBUG
	static constexpr unsigned int DERIVATION_COST = 0x2000;
	#else
	static constexpr unsigned int DERIVATION_COST = 0x4000;
	#endif
#else // PBKDF2
	static constexpr unsigned int KEY_ITERATIONS = 0x10000;
#endif
	static constexpr unsigned int MINIMUM_CIPHERTEXT_LENGTH = MAX_PADDING_BYTES + IV_SIZE + IV_SEED_SIZE + HMAC<SHA256>::DIGESTSIZE;		// 16 bytes padded plaintext, 32 bytes MAC, 8 bytes IV seed

	// provides an upper limit, but not an exact limit, to the length of the ciphertext
	// that will be produced when encrypting plaintext of a specified size.
	static unsigned int MaxCiphertextLen(unsigned int plaintextLen) { return plaintextLen + MINIMUM_CIPHERTEXT_LENGTH; }

	// encryption:
	// use PKCS#7 padding to align to block size for AES CBC mode
	// derive a key from a salted passphrase using PBKDF2 or SCrypt
	// encrypt-then-mac using HMAC-SHA256
	// MAC is generated over ciphertext and IV
	// before: use MaxCiphertextLen() to allocate an output buffer of appropriate size
	static unsigned int Encrypt(RandomNumberGenerator& rng, ConstByteArrayParameter const& passphrase, byte* output, const std::string& plaintext);

	// decryption:
	// derive a key from a salted passphrase using PBKDF2 or SCrypt
	// check MAC tag by generating a HMAC-SHA256 over ciphertext and IV
	// then decrypt and remove padding
	// before: allocate an output buffer that is as large as the input
	static DecodingResult Decrypt(ConstByteArrayParameter const& passphrase, byte* output, ConstByteArrayParameter const& input);

};

NAMESPACE_END

#endif // _AESLAYER_H_
