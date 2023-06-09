#include "encoding.h"

#include <QDebug>

std::map<EncodingType, float> EncodedSignal::_encoding_freq_mult =
{
    { EncodingType::NRZ, 0.5 },
    { EncodingType::RZ, 1 },
    { EncodingType::M2, 1 },
    { EncodingType::MLT3, 0.5 },
    { EncodingType::AMI, 0.5 },
    { EncodingType::NRZI, 0.5 }
};

std::map<EncodingType, EncodedSignal::encoding_t> EncodedSignal::_encoding =
{
    { EncodingType::NRZ, &encodeNRZ },
    { EncodingType::RZ, &encodeRZ },
    { EncodingType::M2, &encodeM2 },
    { EncodingType::MLT3, &encodeMLT3 },
    { EncodingType::AMI, &encodeAMI },
    { EncodingType::NRZI, &encodeNRZI }
};

std::vector<level_t> EncodedSignal::encodeNRZ(const BinData& data)
{
    std::list<level_t> list;

    for (const BinValue& value : data)
    {
        list.push_back(value == BinValue::BIN_ZERO ? -1 : 1);
    }

    return std::vector<level_t>(list.begin(), list.end());
}

std::vector<level_t> EncodedSignal::encodeRZ(const BinData& data)
{
    std::list<level_t> list;

    for (const BinValue& value : data)
    {
        list.push_back(value == BinValue::BIN_ZERO ? -1 : 1);
        list.push_back(0);
    }

    return std::vector<level_t>(list.begin(), list.end());
}

std::vector<level_t> EncodedSignal::encodeM2(const BinData& data)
{
    std::list<level_t> list;

    for (const BinValue& value : data)
    {
        list.push_back(value == BinValue::BIN_ZERO ? -1 : 1);
        list.push_back(value == BinValue::BIN_ZERO ? 1 : -1);
    }

    return std::vector<level_t>(list.begin(), list.end());
}

std::vector<level_t> EncodedSignal::encodeMLT3(const BinData& data)
{
    std::list<level_t> list;

    level_t last_level = 1;
    level_t level = 0;

    for (const BinValue& value : data)
    {
        if (value == BinValue::BIN_ONE)
        {
            switch(level)
            {
            case -1:
                level = 0;
                break;
            case 0:
                level = last_level;
                last_level = -last_level;
                break;
            case 1:
                level = 0;
                break;
            default:
                break;
            }

        }

        list.push_back(level);
    }

    return std::vector<level_t>(list.begin(), list.end());
}

std::vector<level_t> EncodedSignal::encodeAMI(const BinData& data)
{
    std::list<level_t> list;

    level_t last = -1;

    for (const BinValue& value : data)
    {
        if (value == BinValue::BIN_ZERO)
        {
            list.push_back(0);
        }
        else
        {
            last = -last;
            list.push_back(last);
        }
    }

    return std::vector<level_t>(list.begin(), list.end());
}

std::vector<level_t> EncodedSignal::encodeNRZI(const BinData& data)
{
    std::list<level_t> list;

    level_t last = -1;

    for (const BinValue& value : data)
    {
        if (value == BinValue::BIN_ZERO)
        {
            list.push_back(last);
        }
        else
        {
            last = -last;
            list.push_back(last);
        }
    }

    return std::vector<level_t>(list.begin(), list.end());
}

EncodedSignal::EncodedSignal():
    _lowest(0.0),
    _lowest_fourier(0.0),
    _average(0.0),
    _highest(0.0),
    _type(EncodingType::RZ),
    _signals()
{
}

EncodedSignal::EncodedSignal(const EncodedSignal& other):
    _lowest(other._lowest),
    _lowest_fourier(other._lowest_fourier),
    _average(other._average),
    _highest(other._highest),
    _type(other._type),
    _signals(other._signals)
{
}

EncodedSignal::~EncodedSignal()
{
}

EncodedSignal& EncodedSignal::operator=(const EncodedSignal& other)
{
    _lowest = other._lowest;
    _lowest_fourier = other._lowest_fourier;
    _average = other._average;
    _highest = other._highest;
    _type = other._type;
    _signals = other._signals;

    return *this;
}

double EncodedSignal::lowest() const
{
    return _lowest;
}

double EncodedSignal::lowestFourier() const
{
    return _lowest_fourier;
}

double EncodedSignal::average() const
{
    return _average;
}

double EncodedSignal::highest() const
{
    return _highest;
}

EncodingType EncodedSignal::type() const
{
    return _type;
}

const std::vector<level_t> &EncodedSignal::encodedSignal() const
{
    return _signals;
}

void EncodedSignal::reset()
{
    _lowest = 0;
    _lowest_fourier = 0;
    _average = 0;
    _highest = 0;
    _signals = std::vector<level_t>();
}

struct Section
{
    bool is_first;
    bool is_last;
    level_t level;
    size_t length;
};

std::tuple<double, double, double> analyze(EncodingType type, std::vector<level_t> signal)
{
    if (signal.size() == 0) return std::tuple<double, double, double>(0, 0, 0);

    if (type == EncodingType::RZ)
    {
        std::vector<level_t> v;

        for (level_t l : signal)
        {
            if (l != 0) v.push_back(l);
        }

        signal = v;
    }

    size_t last_index = 0;
    level_t last = signal[0];

    Section curr = { .is_first = true, .is_last = true, .level = signal[0], .length = signal.size() };
    std::list<Section> sections;

    for (size_t i = 0; i != signal.size(); i++)
    {
        if (last != signal[i])
        {
            curr.is_last = (i == signal.size() - 1);
            curr.length = i - last_index;

            sections.push_back(curr);

            curr.is_first = false;
            curr.level = signal[i];

            last_index = i;
            last = signal[i];
        }
    }


    curr.is_last = true;
    curr.length = signal.size() - last_index;

    sections.push_back(curr);

    std::vector<Section> ordered(sections.begin(), sections.end()), sorted(sections.begin(), sections.end());

    std::sort(sorted.begin(), sorted.end(), [](const Section& a, const Section& b) -> bool { return a.length < b.length; });

    double longest = 0, shortest = 0, average = 0;

    switch(type)
    {
    //freq low is / 2
    case EncodingType::RZ:
        shortest = 0.5;
        longest = 0.5;

        if (sections.size() != 1)
        {
            longest = 1;
        }

        if (sections.size() == 1)
        {
            average = 1.f;
        }
        else
        {
            average = (double)(2 * signal.size() * 1.f - (sections.size() - 1)) / (double)(2 * signal.size());
        }

        break;
    case EncodingType::NRZ:
        shortest = sorted[0].length;
        longest = sorted[sorted.size() - 1].length;

        average = (double)sections.size() / (2. * (double)signal.size());

        break;
    case EncodingType::M2:
        shortest = sorted[sorted.size() - 1].length;
        longest = sorted[sorted.size() - 1].length;

        for (Section s : sorted)
        {
            if (s.length != longest && !s.is_first && !s.is_last)
            {
                shortest = s.length;
                break;
            }
        }


        shortest /= 2;
        longest /= 2;
        average = (double)(sections.size() - 2) / (double)(signal.size() - 2);

        break;
    case EncodingType::AMI:
        for (size_t i = 0; i != sorted.size(); i++)
        {
            if (sorted[i].level != 0)
            {
                shortest = sorted[i].length;
                break;
            }
        }

        for (size_t i = 0; i != sorted.size(); i++)
        {
            longest = sorted[sorted.size() - 1 - i].length;
            break;
        }

        average = (double)sections.size() / (2. * (double)signal.size());

        break;
    case EncodingType::MLT3:
        for (size_t i = 0; i != sorted.size(); i++)
        {
            shortest = sorted[i].length;
            break;
        }

        for (size_t i = 0; i != sorted.size(); i++)
        {
            longest = sorted[sorted.size() - 1 - i].length;
            break;
        }

        average = (double)sections.size() / (2. * (double)signal.size());

        break;
    case EncodingType::NRZI:
        shortest = sorted[0].length;
        longest = sorted[sorted.size() - 1].length;
\
        average = (double)sections.size() / (2. * (double)signal.size());

        break;
    }

    return std::tuple<double, double, double>(1.0 / (2 * longest), 1.0 / (2 * shortest), average);
}

double fourier(EncodingType type, std::vector<level_t> signal)
{
    const int N = signal.size();

    for (int k = 1; k <= N; k++)
    {
        double a = 0;
        double b = 0;

        for (size_t i = 0; i != signal.size(); i++)
        {
            a += (double)signal[i] * (sin(2 * M_PI * k * (i + 1) / N) - sin(2 * M_PI * k * i / N)) / (M_PI * k);
            b += (double)signal[i] * (cos(2 * M_PI * k * (i + 1) / N) - cos(2 * M_PI * k * i / N)) / (M_PI * k);
        }

        const double r = sqrt(a * a + b * b);

        if (r >= 1e-2)
        {
            return (type == EncodingType::RZ || type == EncodingType::M2 ? 2.0 : 1.0) * ((double)k / (double)N);
        }
    }

    return 0;
}

EncodedSignal EncodedSignal::encode(const BinData& data, EncodingType type)
{
    EncodedSignal signal;

    signal._type = type;
    signal._signals = _encoding[type](data);

    std::tuple<double, double, double> a = analyze(type, signal._signals);

    signal._lowest = std::get<0>(a);
    signal._highest = std::get<1>(a);
    signal._average = std::get<2>(a);
    signal._lowest_fourier = fourier(type, signal._signals);

    return signal;
}
