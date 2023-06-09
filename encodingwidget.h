#ifndef ENCODINGWIDGET_H
#define ENCODINGWIDGET_H

#include "logicencoding.h"

#include <memory>

#include <QLineEdit>
#include <QLabel>
#include <QWidget>

namespace Ui {
class EncodingWidget;
}

class EncodingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EncodingWidget(QWidget *parent = nullptr);
    virtual ~EncodingWidget();

    void setLogicEncoding(std::unique_ptr<LogicEncoding> encoding);
    void setEncodingsList(const std::vector<EncodingType>& encodings);
    void setBandwidth(double bandwidth);
    void setData(const BinData& data);

    void recountFreq();

private:
    void recountFreq(double value, QLineEdit* edit, QLabel* label);

private:
    double _bandwidth;
    BinData _data;
    BinData _encoded;
    EncodingType _current;
    std::vector<EncodingType> _encodings;
    std::unique_ptr<LogicEncoding> _encoding;
    EncodedSignal _signal;
    Ui::EncodingWidget *_ui;
};

#endif // ENCODINGWIDGET_H
