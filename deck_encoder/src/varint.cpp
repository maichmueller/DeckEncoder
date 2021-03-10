
#include "../include/deck_codec/varint.h"

int Varint::pop_varint(std::string &bytes)
{
   ulong result = 0;
   int current_shift = 0;
   int bytes_popped = 0;

   for(int i = 0; i < bytes.size(); i++) {
      bytes_popped++;
      ulong current = static_cast< ulong >(bytes[i]) & AllButMSB;
      result |= current << current_shift;

      if((bytes[i] & JustMSB) != JustMSB) {
         bytes.erase(bytes.begin(), std::next(bytes.begin(), bytes_popped));
         return result;
      }

      current_shift += 7;
   }

   throw std::invalid_argument("Byte array did not contain valid varints.");
}
Varint Varint::from_int(ulong value)
{
   std::vector< byte > buff(10);
   size_t curr_idx = 0;

   if(value == 0)
      // constrcut with 1 element which holds 0 as value
      return Varint(1, 0);

   while(value != 0) {
      auto byte_val = value & AllButMSB;
      value >>= 7;

      if(value != 0)
         byte_val |= 0x80;

      buff[curr_idx++] = static_cast< byte >(byte_val);
   }

   return Varint(buff.begin(), std::next(buff.begin(), curr_idx));
}
