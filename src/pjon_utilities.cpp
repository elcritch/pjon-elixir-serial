
// just cause -- might run this on embedded devices...
#define IS_LITTLE_ENDIAN (!(*(uint16_t *)"\0\xff" < 0x100))

template <typename T>
T swap_endian(T u)
{
  static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

  union
  {
    T u;
    byte u8[sizeof(T)];
  } source, dest;

  source.u = u;

  for (size_t k = 0; k < sizeof(T); k++)
    {
      dest.u8[k] = source.u8[sizeof(T) - k - 1];
    }

  return dest.u;
}
