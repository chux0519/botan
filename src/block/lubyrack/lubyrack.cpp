/*
* Luby-Rackoff
* (C) 1999-2008 Jack Lloyd
*
* Distributed under the terms of the Botan license
*/

#include <botan/lubyrack.h>
#include <botan/internal/xor_buf.h>

namespace Botan {

/*
* Luby-Rackoff Encryption
*/
void LubyRackoff::encrypt_n(const byte in[], byte out[], u32bit blocks) const
   {
   for(u32bit i = 0; i != blocks; ++i)
      {
      const u32bit len = hash->OUTPUT_LENGTH;

      SecureVector<byte> buffer(len);
      hash->update(K1);
      hash->update(in, len);
      hash->final(buffer);
      xor_buf(out + len, in + len, buffer, len);

      hash->update(K2);
      hash->update(out + len, len);
      hash->final(buffer);
      xor_buf(out, in, buffer, len);

      hash->update(K1);
      hash->update(out, len);
      hash->final(buffer);
      xor_buf(out + len, buffer, len);

      hash->update(K2);
      hash->update(out + len, len);
      hash->final(buffer);
      xor_buf(out, buffer, len);

      in += BLOCK_SIZE;
      out += BLOCK_SIZE;
      }
   }

/*
* Luby-Rackoff Decryption
*/
void LubyRackoff::decrypt_n(const byte in[], byte out[], u32bit blocks) const
   {
   for(u32bit i = 0; i != blocks; ++i)
      {
      const u32bit len = hash->OUTPUT_LENGTH;

      SecureVector<byte> buffer(len);
      hash->update(K2);
      hash->update(in + len, len);
      hash->final(buffer);
      xor_buf(out, in, buffer, len);

      hash->update(K1);
      hash->update(out, len);
      hash->final(buffer);
      xor_buf(out + len, in + len, buffer, len);

      hash->update(K2);
      hash->update(out + len, len);
      hash->final(buffer);
      xor_buf(out, buffer, len);

      hash->update(K1);
      hash->update(out, len);
      hash->final(buffer);
      xor_buf(out + len, buffer, len);

      in += BLOCK_SIZE;
      out += BLOCK_SIZE;
      }
   }

/*
* Luby-Rackoff Key Schedule
*/
void LubyRackoff::key_schedule(const byte key[], u32bit length)
   {
   K1.set(key, length / 2);
   K2.set(key + length / 2, length / 2);
   }

/*
* Clear memory of sensitive data
*/
void LubyRackoff::clear()
   {
   zeroise(K1);
   zeroise(K2);
   hash->clear();
   }

/*
* Return a clone of this object
*/
BlockCipher* LubyRackoff::clone() const
   {
   return new LubyRackoff(hash->clone());
   }

/*
* Return the name of this type
*/
std::string LubyRackoff::name() const
   {
   return "Luby-Rackoff(" + hash->name() + ")";
   }

/*
* Luby-Rackoff Constructor
*/
LubyRackoff::LubyRackoff(HashFunction* h) :
   BlockCipher(2 * (h ? h->OUTPUT_LENGTH: 0),
               2, 32, 2),
   hash(h)
   {
   }

}
