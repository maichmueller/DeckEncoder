
#ifndef LORDECKENCODER_UTILS_H
#define LORDECKENCODER_UTILS_H

template < typename T >
struct reversion_wrapper {
   T& iterable;
   auto begin() { return std::rbegin(iterable); }
   auto end() { return std::rend(iterable); }
};
template < typename T >
reversion_wrapper< T > reverse(T&& iterable)
{
   return {iterable};
}

#endif  // LORDECKENCODER_UTILS_H
