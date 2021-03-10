

#ifndef LORDECKENCODER_STRING_UTILS_H
#define LORDECKENCODER_STRING_UTILS_H

#include <algorithm>
#include <string>

namespace string_utils {

// trim from start (in place)
inline std::string &ltrim(std::string &s)
{
   s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
              return ! std::isspace(ch);
           }));
   return s;
}
inline std::string ltrim(std::string &&s)
{
   return ltrim(s);
}

// trim from end (in place)
inline std::string &rtrim(std::string &s)
{
   s.erase(
      std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return std::isspace(ch) == 0; })
         .base(),
      s.end());
   return s;
}
inline std::string rtrim(std::string &&s)
{
   return rtrim(s);
}

// trim from both ends (in place)
inline std::string &trim(std::string &s)
{
   ltrim(s);
   rtrim(s);
   return s;
}
inline std::string trim(std::string &&s)
{
   return trim(s);
}

// pad from left (in place)
inline std::string &pad_left(std::string &s, size_t pad_to_length, char pad_chars)
{
   if(auto diff = pad_to_length - s.size(); diff > 0) {
      s.insert(0, diff, pad_chars);
   }
   return s;
}
// pad from left (in place)
inline std::string pad_left(std::string &&s, size_t pad_to_length, char pad_chars)
{
   // s is an lvalue within this function
   return pad_left(s, pad_to_length, pad_chars);
}
// pad from right (in place)
inline std::string &pad_right(std::string &s, size_t pad_to_length, char pad_chars)
{
   if(auto diff = pad_to_length - s.size(); diff > 0) {
      s.append(diff, pad_chars);
   }
   return s;
}
// pad from right (in place)
inline std::string pad_right(std::string &&s, size_t pad_to_length, char pad_chars)
{
   // s is an lvalue within this function
   return pad_right(s, pad_to_length, pad_chars);
}

}  // namespace string_utils

#endif  // LORDECKENCODER_STRING_UTILS_H
