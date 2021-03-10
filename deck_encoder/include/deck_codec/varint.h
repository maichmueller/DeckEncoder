
#ifndef LORDECKENCODER_VARINT_H
#define LORDECKENCODER_VARINT_H

#include <algorithm>
#include <cstdint>
#include <vector>

class Varint {
   using byte = int8_t;
   using ulong = uint64_t;

   static const byte AllButMSB = 0x7f;
   static const byte JustMSB = 0x80;

   Varint() = default;

   // allow any constructor that the single data member would take
   template < typename... Args >
   explicit Varint(Args&&... args) : m_data(std::forward< Args >(args)...)
   {
   }

  public:
   static int pop_varint(std::string& bytes);
   static Varint from_int(ulong value);

   [[nodiscard]] auto size() const { return m_data.size(); }
   [[nodiscard]] auto begin() const { return m_data.begin(); }
   [[nodiscard]] auto begin() { return m_data.begin(); }
   [[nodiscard]] auto end() const { return m_data.end(); }
   [[nodiscard]] auto end() { return m_data.end(); }
   [[nodiscard]] inline auto& operator[](size_t n) { return m_data[n]; }
   [[nodiscard]] inline auto& operator[](size_t n) const { return m_data[n]; }
   [[nodiscard]] inline const auto& at(size_t idx) const { return m_data.at(idx); }
   [[nodiscard]] inline auto& at(size_t idx) { return m_data.at(idx); }
   template < typename IterType >
   [[nodiscard]] inline auto erase(IterType position)
   {
      return m_data.erase(position);
   }

  private:
   std::vector< byte > m_data;
};

#endif  // LORDECKENCODER_VARINT_H
