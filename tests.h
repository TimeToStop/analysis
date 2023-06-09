#ifndef TESTS_H
#define TESTS_H

#include <QObject>

class LogicTest : public QObject
{
    Q_OBJECT

public:
    explicit LogicTest(QObject* parent = nullptr);
    virtual ~LogicTest();

private slots:
    void excess45_data();
    void scramble35_data();
    void scramble57_data();

    void excess45();
    void scramble35();
    void scramble57();
};

#endif // TESTS_H
