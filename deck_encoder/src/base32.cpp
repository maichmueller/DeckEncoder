
#include "deck_codec/base32.h"

const std::map< char, int > base32::CHAR_MAP = _build_char_map();

int32_t base32::_nr_trailing_zeros(int32_t i)
{
   if(i == 0)
      return 32;
   int32_t n = 31;
   int32_t y = i << 16;
   if(y != 0) {
      n = n - 16;
      i = y;
   }
   y = i << 8;
   if(y != 0) {
      n = n - 8;
      i = y;
   }
   y = i << 4;
   if(y != 0) {
      n = n - 4;
      i = y;
   }
   y = i << 2;
   if(y != 0) {
      n = n - 2;
      i = y;
   }
   return n - static_cast< int32_t >((static_cast< uint32_t >(i << 1) >> 31));
}
std::map< char, int > base32::_build_char_map()
{
   std::map< char, int > m;
   for(int i = 0; i < std::char_traits< char >::length(DIGITS); i++) {
      m[DIGITS[i]] = i;
   }
   return m;
}
std::string base32::decode(std::string code)
{
   // Remove whitespace and separators
   code = string_utils::trim(code);
   std::replace(code.begin(), code.end(), *SEPARATOR, *"");

   // Remove padding. Note: the padding is used as hint to determine how many
   // bits to decode from the last incomplete chunk (which is commented out
   // below, so this may have been wrong to start with).
   std::regex_replace(code, std::regex("[=]*$"), "");

   // Canonicalize to all upper case
   std::transform(code.begin(), code.end(), code.begin(), [](auto c) {
      return static_cast< char >(std::toupper(static_cast< unsigned char >(c)));
   });
   if(code.empty()) {
      return {};
   }
   size_t encoded_len = code.size();
   size_t out_len = encoded_len * SHIFT / 8;
   std::vector< byte > result;
   result.reserve(out_len);
   int buffer = 0;
   int bits_left = 0;
   for(char c : code) {
      auto c_iter = CHAR_MAP.find(c);
      if(c_iter == CHAR_MAP.end()) {
         // char map doesn't hold character c
         throw std::invalid_argument(std::string("Illegal character: ") + c);
      }
      buffer <<= SHIFT;
      buffer |= c_iter->second & MASK;
      bits_left += SHIFT;
      if(bits_left >= 8) {
         result.emplace_back(buffer >> (bits_left - 8));
         bits_left -= 8;
      }
   }
   // We'll ignore leftover bits for now.
   //
   // if (next != out_len || bits_left >= SHIFT) {
   //  throw std::invalid_argument("Bits left: " + bits_left);
   // }
   return {result.begin(), result.end()};
}
std::string base32::encode(const std::string &text, bool pad_output)
{
   static uint8_t byte_len = 8;
   if(text.empty()) {
      return "";
   }

   // SHIFT is the number of bits per output character, so the length of the
   // output is the length of the input multiplied by 8/SHIFT, rounded up.
   if(text.size() >= (1 << 28)) {
      // The computation below will fail, so don't do it.
      throw std::out_of_range("text");
   }

   std::stringstream result;

   int buffer = text[0];
   int next = 1;
   int bitsLeft = byte_len;
   while(bitsLeft > 0 || next < text.size()) {
      if(bitsLeft < SHIFT) {
         if(next < text.size()) {
            buffer <<= 8;
            buffer |= (text[next++] & 0xff);
            bitsLeft += 8;
         } else {
            size_t pad = SHIFT - bitsLeft;
            buffer <<= pad;
            bitsLeft += pad;
         }
      }
      size_t index = MASK & (buffer >> (bitsLeft - SHIFT));
      bitsLeft -= SHIFT;
      result << DIGITS[index];
   }
   if(pad_output) {
      if(size_t padding = byte_len - (result.tellp() % byte_len); padding > 0) {
         result << std::string(padding == byte_len ? 0 : padding, '=');
      };
   }
   return result.str();
}
