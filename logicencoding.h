#ifndef LOGICENCODING_H
#define LOGICENCODING_H

#include "encoding.h"

class LogicEncoding
{
public:
    LogicEncoding();
    ~LogicEncoding();

    virtual BinData encode(const BinData& data) = 0;
};

class IdentityEncoding : public LogicEncoding
{
public:
    IdentityEncoding();
    ~IdentityEncoding();

    virtual BinData encode(const BinData& data);
};

class ExcessEncoding : public LogicEncoding
{
public:
    ExcessEncoding();
    ~ExcessEncoding();

    virtual BinData encode(const BinData& data);
};

class Scramble35Encoding : public LogicEncoding
{
public:
    Scramble35Encoding();
    ~Scramble35Encoding();

    virtual BinData encode(const BinData& data);
};

class Scramble57Encoding : public LogicEncoding
{
public:
    Scramble57Encoding();
    ~Scramble57Encoding();

    virtual BinData encode(const BinData& data);
};

#endif // LOGICENCODING_H
