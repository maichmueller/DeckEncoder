

#ifndef LORDECKENCODER_DECK_DESIGN_H
#define LORDECKENCODER_DECK_DESIGN_H

class CardToken {
  public:
   CardToken(std::string code, size_t count) : m_code(std::move(code)), m_count(count){};

   [[nodiscard]] auto code() const { return m_code; }
   [[nodiscard]] auto count() const { return m_count; }

   bool operator==(const CardToken &other) const
   {
      return m_code == other.code() && m_count == other.count();
   }
   bool operator!=(const CardToken &other) const { return not (*this == other); }

  private:
   std::string m_code;
   size_t m_count;
};

#endif  // LORDECKENCODER_DECK_DESIGN_H
