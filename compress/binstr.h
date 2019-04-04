#ifndef _BINSTR_H_
#define _BINSTR_H_
#include <cstdint>

class BinStr {
    size_t arr_size, bit_size;
    uint8_t *buf;
public:
    BinStr();
    BinStr(uint8_t *buf, size_t size);
    BinStr(const BinStr& bs);
    ~BinStr();
    size_t get_bit_size() const;
    uint32_t substr(size_t startpos, size_t len) const;
    void push_back(uint32_t val, size_t len);
    void pop_bit(size_t cnt=1);
    void push_back(const BinStr& bs);
    void fill();
    void unfill();
    char* copy_to_buffer(size_t & cnt_bytes);
};

#endif
