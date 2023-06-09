#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    QDoubleValidator* validator = new QDoubleValidator(this);

    validator->setBottom(0);
    validator->setLocale(QLocale("en_US"));

    _ui->bandwidth->setValidator(validator);

    _ui->normal->setBandwidth(10);
    _ui->excess->setBandwidth(10);
    _ui->scramble35->setBandwidth(10);
    _ui->scramble57->setBandwidth(10);

    _ui->normal->setLogicEncoding(std::make_unique<IdentityEncoding>());
    _ui->excess->setLogicEncoding(std::make_unique<ExcessEncoding>());
    _ui->scramble35->setLogicEncoding(std::make_unique<Scramble35Encoding>());
    _ui->scramble57->setLogicEncoding(std::make_unique<Scramble57Encoding>());

    _ui->normal->setEncodingsList({ EncodingType::RZ, EncodingType::NRZ, EncodingType::AMI,
                                    EncodingType::MLT3, EncodingType::NRZI, EncodingType::M2 });
    _ui->excess->setEncodingsList({ EncodingType::NRZ, EncodingType::AMI,
                                    EncodingType::MLT3, EncodingType::NRZI });
    _ui->scramble35->setEncodingsList({ EncodingType::NRZ, EncodingType::AMI,
                                        EncodingType::MLT3, EncodingType::NRZI });
    _ui->scramble57->setEncodingsList({ EncodingType::NRZ, EncodingType::AMI,
                                        EncodingType::MLT3, EncodingType::NRZI });

    connect(_ui->bandwidth_dim,
            &QComboBox::currentIndexChanged,
            this,
            [=](int index)
            {
                const double bandwidth = _ui->bandwidth->text().toDouble() * std::pow(10.0, 3 * index);

                _ui->normal->setBandwidth(bandwidth);
                _ui->excess->setBandwidth(bandwidth);
                _ui->scramble35->setBandwidth(bandwidth);
                _ui->scramble57->setBandwidth(bandwidth);
            }
    );

    connect(_ui->bandwidth,
            &QLineEdit::textChanged,
            this,
            [=](const QString& value)
            {
                const double bandwidth = value.toDouble() * std::pow(10.0, 3 * _ui->bandwidth_dim->currentIndex());

                _ui->normal->setBandwidth(bandwidth);
                _ui->excess->setBandwidth(bandwidth);
                _ui->scramble35->setBandwidth(bandwidth);
                _ui->scramble57->setBandwidth(bandwidth);
            }
    );

    connect(_ui->message,
            &QLineEdit::textChanged,
            this,
            [=](const QString& message)
            {
                BinData data;
                QTextCodec* codec = QTextCodec::codecForName("CP1251");
                QByteArray bytes = codec->fromUnicode(message);

                for (unsigned char c : bytes)
                {
                    for (int i = 0; i != 8; i++)
                    {
                        const int bit = (c >> (7 - i)) & 1;

                        data.push_back(bit == 0 ? BinValue::BIN_ZERO : BinValue::BIN_ONE);
                    }
                }

                _ui->normal->setData(data);
                _ui->excess->setData(data);
                _ui->scramble35->setData(data);
                _ui->scramble57->setData(data);
            }
    );
}

MainWindow::~MainWindow()
{
    delete _ui;
}

