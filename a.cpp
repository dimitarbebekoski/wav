#include <array>
#include <charconv>
#include <fstream>
#include <iostream>
#include <cstddef>

constexpr int value = 15;
constexpr int value2 = 16;

template <typename T> std::array<unsigned char, sizeof(T)> to_bytes(const T &object)
{
    std::array<unsigned char, sizeof(T)> bytes;

    const unsigned char *begin = reinterpret_cast<const unsigned char *>(std::addressof(object));
    const unsigned char *end = begin + sizeof(T);
    std::copy(begin, end, std::begin(bytes));

    return bytes;
}

int main()
{
    auto arr = to_bytes(value);
    auto arr2 = to_bytes(value2);

    std::ofstream ofs("test.wav");
    ofs.write(reinterpret_cast<const char *>(&value), 4);
    ofs.write(reinterpret_cast<const char *>(&value2), 4);
    ofs.close();

    ofs.open("test2.wav");
    for (size_t i = 0; i < arr.size(); ++i)
        ofs << arr.at(i);
    for (size_t i = 0; i < arr2.size(); ++i)
        ofs << arr2.at(i);
    ofs.close();

    std::byte byte;
    //ofs.write(byte, 1);
    

    return 0;
}
