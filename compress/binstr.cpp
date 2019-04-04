#include <cstring>
#include "binstr.h"


BinStr::BinStr() {
    arr_size = 10;
    bit_size = 0;
    buf = new uint8_t[arr_size];
}


BinStr::BinStr(uint8_t *src, size_t src_bytes) {
    arr_size = src_bytes + 10;
    bit_size = (src_bytes << 3);
    buf = new uint8_t[arr_size];
    memcpy(buf, src, src_bytes);
}


BinStr::BinStr(const BinStr& bs) {
    arr_size = bs.arr_size;
    bit_size = bs.bit_size;
    buf = new uint8_t[arr_size];
    memcpy(buf, bs.buf, ((bit_size + 7) >> 3));
}


BinStr::~BinStr() {
    delete[] buf;
}


size_t BinStr::get_bit_size() const {
    return bit_size;
}


uint32_t BinStr::substr(size_t startpos, size_t len) const {
    size_t seg = startpos >> 3, off = startpos & 7, tail = (off + len + 7) >> 3;
    uint64_t data = 0, mask = ((1ull << len) - 1);
    for (size_t i = 0; i < tail; i++) {
        data |= buf[seg + i] << (i << 3);
    }
    data >>= off;
    return data & mask;
}


void BinStr::push_back(uint32_t val, size_t len) {
    size_t seg = bit_size >> 3, off = bit_size & 7, tail = (off + len + 7) >> 3;
    if (seg + tail > arr_size) {
        size_t new_size = (seg + tail) << 1;
        uint8_t *new_buf = new uint8_t[new_size];
        memcpy(new_buf, buf, arr_size);
        delete[] buf;
        arr_size = new_size;
        buf = new_buf;
    }
    uint64_t data = val, mask = ((1ull << off) - 1);
    data <<= off;
    data |= buf[seg] & mask;
    for (size_t i = 0; i < tail; i++) {
        buf[seg + i] = data >> (i << 3);
    }
    bit_size += len;
}


void BinStr::push_back(const BinStr& bs) {
    size_t len = bs.bit_size;
    uint32_t *ptr = (uint32_t*)bs.buf;
    while (len) {
        size_t seglen = len < 32 ? len : 32;
        push_back(*ptr, seglen);
        len -= seglen;
        ptr++;
    }
}


void BinStr::pop_bit(size_t cnt) {
    bit_size -= cnt;
}


void BinStr::fill() {
    size_t fill = bit_size & 7;
    if (fill) {
        fill = 8 - fill;
        push_back(0, fill);
    }
    push_back(fill, 8);
}


void BinStr::unfill() {
    // assert input.get_bit_size() & 7 == 0
    bit_size -= (buf[(bit_size >> 3) - 1] + 8);
}


char* BinStr::copy_to_buffer(size_t & cnt_bytes) {
    cnt_bytes = (bit_size + 7) >> 3;
    char* ret = new char[cnt_bytes + 1];
    memcpy(ret, buf, cnt_bytes);
    ret[cnt_bytes] = 0;
    return ret;
}
