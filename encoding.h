#ifndef ENCODING_H
#define ENCODING_H

#include <vector>
#include <map>

enum class EncodingType
{
    NRZ,
    RZ,
    M2,
    MLT3,
    AMI,
    NRZI
};

enum class BinValue
{
    BIN_ZERO = 0,
    BIN_ONE = 1
};

using level_t = int;
using BinData = std::vector<BinValue>;

class EncodedSignal
{
private:
    using encoding_t = std::vector<level_t> (*)(const BinData& sequence);

    static std::map<EncodingType, float> _encoding_freq_mult;
    static std::map<EncodingType, encoding_t> _encoding;

    static std::vector<level_t> encodeNRZ(const BinData& data);
    static std::vector<level_t> encodeRZ(const BinData& data);
    static std::vector<level_t> encodeM2(const BinData& data);
    static std::vector<level_t> encodeMLT3(const BinData& data);
    static std::vector<level_t> encodeAMI(const BinData& data);
    static std::vector<level_t> encodeNRZI(const BinData& data);

public:
    static EncodedSignal encode(const BinData& data, EncodingType type);

public:
    EncodedSignal();
    EncodedSignal(const EncodedSignal& other);
    ~EncodedSignal();

    EncodedSignal& operator=(const EncodedSignal& other);

    double lowest() const;
    double lowestFourier() const;
    double average() const;
    double highest() const;
    EncodingType type() const;
    const std::vector<level_t>& encodedSignal() const;

    void reset();

private:
    double _lowest;
    double _lowest_fourier;
    double _average;
    double _highest;
    EncodingType _type;
    std::vector<level_t> _signals;
};


#endif // ENCODING_H
