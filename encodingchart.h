#ifndef ENCODINGCHART_H
#define ENCODINGCHART_H

#include "encoding.h"

#include <QPainter>
#include <QWidget>

enum class LevelState
{
    CHANGING = 0,
    CONSTANT
};

struct EncodingInfo
{
    bool _is_border;
    enum LevelState _state;
    size_t _index;
    EncodedSignal* _signal;
};

class EncodingChart : public QWidget
{
    Q_OBJECT

    using Helper = void(*)(QPainter& painter, const EncodingInfo& info);

    static QPen _neutral_pen;
    static QPen _low_pen;
    static QPen _high_pen;
    static QMap<EncodingType, Helper> _helpers;
    static QMap<EncodingType, QString> _label_for;

    static void helperRZ(QPainter& painter, const EncodingInfo& info);
    static void helperNRZ(QPainter& painter, const EncodingInfo& info);
    static void helperM2(QPainter& painter, const EncodingInfo& info);
    static void helperMLT3(QPainter& painter, const EncodingInfo& info);
    static void helperAMI(QPainter& painter, const EncodingInfo& info);
    static void helperNRZI(QPainter& painter, const EncodingInfo& info);

public:
    explicit EncodingChart(QWidget *parent = nullptr);
    virtual ~EncodingChart();

    EncodedSignal signal() const;

    void setEncoding(EncodingType encoding);
    void setData(const BinData& data);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private:
    EncodingType _encoding;
    BinData _data;
    EncodedSignal _signal;
};

#endif // ENCODINGCHART_H
