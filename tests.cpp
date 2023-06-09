#include "tests.h"

#include "logicencoding.h"

#include <QTest>

LogicTest::LogicTest(QObject* parent):
    QObject(parent)
{
}

LogicTest::~LogicTest()
{
}

void LogicTest::excess45_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("expected");

    QTest::newRow("empty") << QString("") << QString("");

    QTest::newRow("less1") << QString("1") << QString("1");
    QTest::newRow("less2") << QString("10") << QString("10");
    QTest::newRow("less3") << QString("010") << QString("010");

    QTest::newRow("symbol1") << QString("0000")  << QString("11110");
    QTest::newRow("symbol2") << QString("0001")  << QString("01001");
    QTest::newRow("symbol3") << QString("0010")  << QString("10100");
    QTest::newRow("symbol4") << QString("0011")  << QString("10101");
    QTest::newRow("symbol5") << QString("0100")  << QString("01010");
    QTest::newRow("symbol6") << QString("0101")  << QString("01011");
    QTest::newRow("symbol7") << QString("0110")  << QString("01110");
    QTest::newRow("symbol8") << QString("0111")  << QString("01111");
    QTest::newRow("symbol9") << QString("1000")  << QString("10010");
    QTest::newRow("symbol10") << QString("1001") << QString("10011");
    QTest::newRow("symbol11") << QString("1010") << QString("10110");
    QTest::newRow("symbol12") << QString("1011") << QString("10111");
    QTest::newRow("symbol13") << QString("1100") << QString("11010");
    QTest::newRow("symbol14") << QString("1101") << QString("11011");
    QTest::newRow("symbol15") << QString("1110") << QString("11100");
    QTest::newRow("symbol16") << QString("1111") << QString("11101");

    QTest::newRow("random1") << QString("11000111") << QString("1101001111");
    QTest::newRow("random2") << QString("11011111") << QString("1101111101");
    QTest::newRow("random3") << QString("11100000") << QString("1110011110");
    QTest::newRow("random4") << QString("11111000") << QString("1110110010");

    QTest::newRow("not round1") << QString("0001011010") << QString("010010111010");
    QTest::newRow("not round2") << QString("1010110011") << QString("101101101011");
    QTest::newRow("not round3") << QString("11101100001") << QString("1110011010001");
}

void LogicTest::scramble35_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("expected");

    QTest::newRow("empty") << QString("") << QString("");

    QTest::newRow("less1") << QString("1") << QString("1");
    QTest::newRow("less2") << QString("10") << QString("10");
    QTest::newRow("less3") << QString("010") << QString("010");

    QTest::newRow("first step1") << QString("0000") << QString("0000");
    QTest::newRow("first step2") << QString("1111") << QString("1110");
    QTest::newRow("first step3") << QString("0101") << QString("0101");

    QTest::newRow("full1") << QString("010101") << QString("010111");
    QTest::newRow("full2") << QString("11111111") << QString("11100100");
    QTest::newRow("full3") << QString("0101010101100010") << QString("0101110000000010");
}

void LogicTest::scramble57_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("expected");

    QTest::newRow("empty") << QString("") << QString("");

    QTest::newRow("less1") << QString("1") << QString("1");
    QTest::newRow("less2") << QString("10") << QString("10");
    QTest::newRow("less3") << QString("010") << QString("010");
    QTest::newRow("less4") << QString("0101") << QString("0101");
    QTest::newRow("less5") << QString("11111") << QString("11111");

    QTest::newRow("first step1") << QString("000001") << QString("000001");
    QTest::newRow("first step2") << QString("111111") << QString("111110");
    QTest::newRow("first step3") << QString("010101") << QString("010101");

    QTest::newRow("full1") << QString("01010101") << QString("01010111");
    QTest::newRow("full2") << QString("11111111") << QString("11111001");
    QTest::newRow("full3") << QString("0101010101100010") << QString("0101011101110111");
}

void LogicTest::excess45()
{
    ExcessEncoding e;

    QFETCH(QString, source);
    QFETCH(QString, expected);

    BinData data;

    for (QChar c : source)
    {
        data.push_back(c.toLatin1() == '0' ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
    }

    QString result_string;
    BinData result = e.encode(data);

    for(BinValue v : result)
    {
        result_string.push_back(v == BinValue::BIN_ZERO ? '0' : '1');
    }

    QCOMPARE(expected.size(), result_string.size());

    for (qsizetype i = 0; i != expected.size(); i++)
    {
        QCOMPARE(expected[i], result_string[i]);
    }
}

void LogicTest::scramble35()
{
    Scramble35Encoding e;

    QFETCH(QString, source);
    QFETCH(QString, expected);

    BinData data;

    for (QChar c : source)
    {
        data.push_back(c.toLatin1() == '0' ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
    }

    QString result_string;
    BinData result = e.encode(data);

    for(BinValue v : result)
    {
        result_string.push_back(v == BinValue::BIN_ZERO ? '0' : '1');
    }

    QCOMPARE(expected.size(), result_string.size());

    for (qsizetype i = 0; i != expected.size(); i++)
    {
        QCOMPARE(expected[i], result_string[i]);
    }
}

void LogicTest::scramble57()
{
    Scramble57Encoding e;

    QFETCH(QString, source);
    QFETCH(QString, expected);

    BinData data;

    for (QChar c : source)
    {
        data.push_back(c.toLatin1() == '0' ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
    }

    QString result_string;
    BinData result = e.encode(data);

    for(BinValue v : result)
    {
        result_string.push_back(v == BinValue::BIN_ZERO ? '0' : '1');
    }

    QCOMPARE(expected.size(), result_string.size());

    for (qsizetype i = 0; i != expected.size(); i++)
    {
        QCOMPARE(expected[i], result_string[i]);
    }
}
