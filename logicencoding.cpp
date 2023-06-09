#include "logicencoding.h"

#include <cstddef>

LogicEncoding::LogicEncoding()
{
}

LogicEncoding::~LogicEncoding()
{
}

IdentityEncoding::IdentityEncoding()
{
}

IdentityEncoding::~IdentityEncoding()
{
}

BinData IdentityEncoding::encode(const BinData& data)
{
    return data;
}

ExcessEncoding::ExcessEncoding()
{
}

ExcessEncoding::~ExcessEncoding()
{
}

int translate_table[]=
{
    0b11110,
    0b01001,
    0b10100,
    0b10101,
    0b01010,
    0b01011,
    0b01110,
    0b01111,
    0b10010,
    0b10011,
    0b10110,
    0b10111,
    0b11010,
    0b11011,
    0b11100,
    0b11101,
};

BinData ExcessEncoding::encode(const BinData& data)
{
    size_t i = 0;
    BinData encoded;

    while (i != data.size())
    {
        if (data.size() - i >= 4)
        {
            const int value =
                    ((data[i + 0] == BinValue::BIN_ZERO ? 0 : 1) << 3)
                  + ((data[i + 1] == BinValue::BIN_ZERO ? 0 : 1) << 2)
                  + ((data[i + 2] == BinValue::BIN_ZERO ? 0 : 1) << 1)
                  + ((data[i + 3] == BinValue::BIN_ZERO ? 0 : 1) << 0);

            const int encoded_value = translate_table[value];

            encoded.push_back(((encoded_value >> 4) & 1) == 0 ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
            encoded.push_back(((encoded_value >> 3) & 1) == 0 ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
            encoded.push_back(((encoded_value >> 2) & 1) == 0 ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
            encoded.push_back(((encoded_value >> 1) & 1) == 0 ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
            encoded.push_back(((encoded_value >> 0) & 1) == 0 ? BinValue::BIN_ZERO : BinValue::BIN_ONE);

            i += 4;
        }
        else
        {
            while (i != data.size())
            {
                encoded.push_back(data[i++]);
            }
        }
    }

    return encoded;
}

Scramble35Encoding::Scramble35Encoding()
{
}

Scramble35Encoding::~Scramble35Encoding()
{
}

BinValue signalXor(BinValue a, BinValue b)
{
    return (a != b) ? BinValue::BIN_ONE : BinValue::BIN_ZERO;
}

BinData Scramble35Encoding::encode(const BinData& data)
{
    BinData encoded;

    for (size_t i = 0; i != data.size(); i++)
    {
        if (i < 3)
        {
            encoded.push_back(data[i]);
        }
        else if (i < 5)
        {
            encoded.push_back(signalXor(data[i], encoded[i - 3]));
        }
        else
        {
            encoded.push_back(signalXor(signalXor(data[i], encoded[i - 3]), encoded[i - 5]));
        }
    }

    return encoded;
}

Scramble57Encoding::Scramble57Encoding()
{
}

Scramble57Encoding::~Scramble57Encoding()
{
}

BinData Scramble57Encoding::encode(const BinData& data)
{
    BinData encoded;

    for (size_t i = 0; i != data.size(); i++)
    {
        if (i < 5)
        {
            encoded.push_back(data[i]);
        }
        else if (i < 7)
        {
            encoded.push_back(signalXor(data[i], encoded[i - 5]));
        }
        else
        {
            encoded.push_back(signalXor(signalXor(data[i], encoded[i - 5]), encoded[i - 7]));
        }
    }

    return encoded;
}
