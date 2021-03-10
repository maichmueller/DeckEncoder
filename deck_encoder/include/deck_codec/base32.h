
#ifndef LORDECKENCODER_BASE32_H
#define LORDECKENCODER_BASE32_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "string_utils.h"

template < size_t v, uint32_t i, uint32_t n >
constexpr static std::array< uint32_t, 2 > move_bits()
{
   constexpr uint32_t t = i << v;
   if constexpr(t != 0) {
      return {t, n - v};
   } else {
      return {i, n};
   }
}

template < uint32_t i >
constexpr static uint32_t nr_trailing_zeros()
{
   if constexpr(i == 0)
      return 32;
   constexpr auto a = move_bits< 16, i, 31 >();
   constexpr auto b = move_bits< 8, a[0], a[1] >();
   constexpr auto c = move_bits< 4, b[0], b[1] >();
   constexpr auto d = move_bits< 2, c[0], c[1] >();
   return d[1] - static_cast< int32_t >((static_cast< uint32_t >(d[0] << 1) >> 31));
}

class base32 {
  public:
   using byte = int8_t;

   static std::string decode(std::string code);
   static std::string encode(const std::string &text, bool pad_output = false);

  private:
   constexpr static const char *DIGITS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
   constexpr static const size_t len = std::char_traits< char >::length(DIGITS);
   constexpr static const size_t MASK = len - 1;
   constexpr static uint32_t SHIFT = nr_trailing_zeros< len >();
   constexpr static const char *SEPARATOR = "-";
   static std::map< char, int > _build_char_map();

   static const std::map< char, int > CHAR_MAP;
   static int32_t _nr_trailing_zeros(int32_t i);
};

#endif  // LORDECKENCODER_BASE32_H
