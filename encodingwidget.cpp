#include "encodingwidget.h"
#include "ui_encodingwidget.h"

EncodingWidget::EncodingWidget(QWidget* parent):
    QWidget(parent),
    _bandwidth(1.0),
    _data(),
    _current(EncodingType::RZ),
    _encodings(),
    _encoding(new IdentityEncoding()),
    _ui(new Ui::EncodingWidget)
{
    _ui->setupUi(this);

    connect(_ui->physic_encoding,
            &QComboBox::currentIndexChanged,
            this,
            [=](int index)
            {
                if (0 <= index && (size_t)index < _encodings.size())
                {
                    _current = _encodings[index];
                    _ui->chart->setEncoding(_encodings[index]);
                    recountFreq();
                }
            }
    );
}

EncodingWidget::~EncodingWidget()
{
    delete _ui;
}

void EncodingWidget::setLogicEncoding(std::unique_ptr<LogicEncoding> encoding)
{
    _encoding.swap(encoding);
}

void EncodingWidget::setEncodingsList(const std::vector<EncodingType>& encodings)
{
    QMap<EncodingType, QString> map =
    {
        { EncodingType::RZ,     "RZ" },
        { EncodingType::NRZ,    "NRZ" },
        { EncodingType::AMI,    "AMI" },
        { EncodingType::MLT3,   "MLT3" },
        { EncodingType::M2,     "M2" },
        { EncodingType::NRZI,   "NRZI" }
    };

    _ui->physic_encoding->clear();

    for (EncodingType encoding : encodings)
    {
         _ui->physic_encoding->addItem(map[encoding]);
    }

    _ui->chart->setEncoding(encodings[0]);

    _current = encodings[0];
    _encodings = encodings;

    recountFreq();
}

void EncodingWidget::setBandwidth(double bandwidth)
{
    _bandwidth = bandwidth;
    recountFreq();
}

void EncodingWidget::setData(const BinData& data)
{
    _data = data;

    if (_data.empty())
    {
        _encoded = BinData();
        _signal.reset();
        _ui->bin_message->setText("");
        _ui->hex_message->setText("");
        _ui->chart->setData(_encoded);
        recountFreq();
        return;
    }

    BinData encoded = _encoding->encode(data);

    _encoded = encoded;

    QString encoded_bin;
    QString encoded_hex;

    int value = 0;
    int counter = 0;

    for (BinValue s : encoded)
    {
        encoded_bin += (s == BinValue::BIN_ZERO ? "0" : "1");
        value += (s == BinValue::BIN_ZERO ? 0 : 1);

        if (++counter == 8)
        {
            counter = 0;
            encoded_bin += " ";
            encoded_hex += QString::number(value, 16) + " ";
            value = 0;
        }

        value <<= 1;
    }

    if (counter != 0)
    {
        encoded_hex += QString::number(value / 2, 16);
    }

    _ui->bin_message->setText(encoded_bin);
    _ui->hex_message->setText(encoded_hex);

    _ui->chart->setData(encoded);

    recountFreq();
}

void EncodingWidget::recountFreq()
{
    _signal = EncodedSignal::encode(_encoded, _current);

    const double lowest = _bandwidth * _signal.lowest();
    const double average = _bandwidth * _signal.average();
    const double highest = _bandwidth * _signal.highest();
    const double lowest_fourier = _bandwidth * _signal.lowestFourier();
    const double spectrum = _bandwidth * (_signal.highest() - _signal.lowest());

    recountFreq(lowest, _ui->low_freq, _ui->low_freq_dim);
    recountFreq(average, _ui->avg_freq, _ui->avg_freq_dim);
    recountFreq(highest, _ui->high_freq, _ui->high_freq_dim);
    recountFreq(lowest_fourier, _ui->low_freq_fourier, _ui->low_freq_fourier_dim);
    recountFreq(spectrum, _ui->spectrum, _ui->spectrum_dim);
}

void EncodingWidget::recountFreq(double value, QLineEdit* edit, QLabel* label)
{
    int i = 0;
    const char* dims[] = { " Гц", "КГц", "МГц", "ГГц" };

    for (; i != 3; i++)
    {
        if (value < 1000)
        {
            break;
        }

        value /= 1000;
    }

    edit->setText(QString::number(value));
    label->setText(dims[i]);
}
