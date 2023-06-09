#include "encodingchart.h"

QPen EncodingChart::_neutral_pen(Qt::black, 3);
QPen EncodingChart::_low_pen(Qt::red, 3);
QPen EncodingChart::_high_pen(Qt::blue, 3);

QMap<EncodingType, EncodingChart::Helper> EncodingChart::_helpers =
{
    { EncodingType::RZ,     &helperRZ   },
    { EncodingType::NRZ,    &helperNRZ  },
    { EncodingType::M2,     &helperM2   },
    { EncodingType::MLT3,   &helperMLT3 },
    { EncodingType::AMI,    &helperAMI  },
    { EncodingType::NRZI,   &helperNRZI }
};

QMap<EncodingType, QString> EncodingChart::_label_for =
{
    { EncodingType::RZ,     "RZ"    },
    { EncodingType::NRZ,    "NRZ"   },
    { EncodingType::M2,     "M2"    },
    { EncodingType::MLT3,   "MLT3"  },
    { EncodingType::AMI,    "AMI"   },
    { EncodingType::NRZI,   "NRZI"  }
};

void EncodingChart::helperRZ(QPainter& painter, const EncodingInfo& info)
{
    if (info._state == LevelState::CONSTANT)
    {
        switch(info._signal->encodedSignal()[info._index])
        {
        case -1:
            painter.setPen(_low_pen);
            break;
        case 0:
            painter.setPen(_neutral_pen);
            break;
        case 1:
            painter.setPen(_high_pen);
            break;
        default:
            break;
        }
    }
}

void EncodingChart::helperNRZ(QPainter& painter, const EncodingInfo& info)
{
    if (info._state == LevelState::CONSTANT)
    {
        switch(info._signal->encodedSignal()[info._index])
        {
        case -1:
            painter.setPen(_low_pen);
            break;
        case 1:
            painter.setPen(_high_pen);
            break;
        default:
            break;
        }
    }
}

void EncodingChart::helperM2(QPainter& painter, const EncodingInfo& info)
{
    painter.setPen(_neutral_pen);

    if (info._state == LevelState::CHANGING && !info._is_border)
    {
        switch(info._signal->encodedSignal()[info._index])
        {
        case 1:
            painter.setPen(_low_pen);
            break;
        case -1:
            painter.setPen(_high_pen);
            break;
        default:
            break;
        }
    }
}

void EncodingChart::helperMLT3(QPainter& painter, const EncodingInfo& info)
{
    painter.setPen(_neutral_pen);

    if (info._state == LevelState::CHANGING)
    {
        painter.setPen(_high_pen);
    }
    else if (   info._index != 0
             && info._signal->encodedSignal()[info._index] == info._signal->encodedSignal()[info._index - 1])
    {
        painter.setPen(_low_pen);
    }
    else if (info._index == 0 && info._signal->encodedSignal()[info._index] == 0)
    {
        painter.setPen(_low_pen);
    }
}

void EncodingChart::helperAMI(QPainter& painter, const EncodingInfo& info)
{
    if (info._state == LevelState::CONSTANT)
    {
        switch(info._signal->encodedSignal()[info._index])
        {
        case 0:
            painter.setPen(_low_pen);
            break;
        default:
            painter.setPen(_high_pen);
            break;
        }
    }
}

void EncodingChart::helperNRZI(QPainter& painter, const EncodingInfo& info)
{
    painter.setPen(_neutral_pen);

    if (info._state == LevelState::CONSTANT)
    {
        if (info._index == 0 && info._signal->encodedSignal()[info._index] == -1)
        {
            painter.setPen(_low_pen);
        }
        else if (info._signal->encodedSignal()[info._index] == info._signal->encodedSignal()[info._index - 1])
        {
            painter.setPen(_low_pen);
        }
    }
    else
    {
        painter.setPen(_high_pen);
    }
}

EncodingChart::EncodingChart(QWidget *parent):
    QWidget(parent),
    _encoding(EncodingType::RZ),
    _data(),
    _signal()
{
}

EncodingChart::~EncodingChart()
{
}

EncodedSignal EncodingChart::signal() const
{
    return _signal;
}

void EncodingChart::setEncoding(EncodingType encoding)
{
    _encoding = encoding;
    _signal = EncodedSignal::encode(_data, _encoding);
    repaint();
}

void EncodingChart::setData(const BinData& data)
{
    _data = data;
    _signal = EncodedSignal::encode(_data, _encoding);
    repaint();
}

void EncodingChart::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
    painter.drawRect(2, 2, width() - 4, height() - 3);

    const int width = 10;
    const int height = 50;
    const int grid_margin = 10;
    const int graph_margin = 40;
    QPoint zero(20 + grid_margin, 30 + height / 2);
//    EncodingType encodings[] = { EncodingType::RZ, EncodingType::NRZ, EncodingType::M2, EncodingType::MLT3, EncodingType::AMI, EncodingType::NRZI };

    EncodingType encoding = _encoding;

//    for (EncodingType encoding : encodings)
    {
        const int levels = 2;
        const int signal_level = height / levels;

        int index = 0;
        int signal_index = 0;
        bool is_first = true;
        level_t prev = 0;
        QPoint prev_point;
        EncodingInfo info;

        const int width_align = width * (_encoding == EncodingType::RZ || _encoding == EncodingType::M2 ? 1 : 2);

        info._index = 0;
        info._state = LevelState::CHANGING;
        info._signal = &_signal;

        // draw name
        painter.setPen(_neutral_pen);
        painter.rotate(-90);
        painter.drawText(QPoint(-zero.y(), -10 + zero.x()), _label_for[encoding]);
        painter.rotate(90);

        int counter = 0;

        for (level_t level : _signal.encodedSignal())
        {
            // draw grid
            painter.setPen(QPen(Qt::black, 1));

            painter.drawLine(zero, zero + QPoint(width_align, 0));

            const int text_shift = 2;
            info._is_border = true;

            if (encoding == EncodingType::RZ || encoding == EncodingType::M2)
            {
                if (counter == 0)
                {
                    painter.setPen(_data[signal_index] == BinValue::BIN_ZERO ? _low_pen : _high_pen);
                    painter.drawText(zero + QPoint(text_shift + width / 2, - height / 2 - grid_margin), _data[signal_index++] == BinValue::BIN_ZERO ? "0" : "1");
                    painter.setPen(QPen(Qt::black, 1));
                    painter.drawLine(zero + QPoint(0, - height / 2 - grid_margin), zero + QPoint(0, height / 2 + grid_margin));
                }
                else
                {
                    info._is_border = false;
                }

                counter = (counter + 1) % 2;
            }
            else
            {
                painter.setPen(_data[signal_index] == BinValue::BIN_ZERO ? _low_pen : _high_pen);
                painter.drawText(zero + QPoint(text_shift + width / 2, - height / 2 - grid_margin), _data[signal_index++] == BinValue::BIN_ZERO ? "0" : "1");
                painter.setPen(QPen(Qt::black, 1));
                painter.drawLine(zero + QPoint(0, - height / 2 - grid_margin), zero + QPoint(0, height / 2 + grid_margin));
            }

            info._index = index++;

            // draw signal
            painter.setPen(QPen(Qt::black, 3));

            if (!is_first && prev != level)
            {
                info._state = LevelState::CHANGING;
                _helpers[encoding](painter, info);
                painter.drawLine(prev_point, zero + QPoint(0, level * -signal_level));
            }

            // SPECIAL FOR MLT3
            if (is_first && encoding == EncodingType::MLT3)
            {
                info._state = LevelState::CHANGING;
                _helpers[encoding](painter, info);
                // TODO: commit that MLT3 starts with +1
                painter.drawLine(zero + QPoint(0, 0), zero + QPoint(0, level * -signal_level));
            }

            info._state = LevelState::CONSTANT;
            _helpers[encoding](painter, info);
            painter.drawLine(zero + QPoint(0, level * -signal_level), zero + QPoint(width_align, level * -signal_level));

            is_first = false;
            prev = level;
            prev_point = zero + QPoint(width_align, level * -signal_level);
            zero.rx() += width_align;
        }

        if (!_signal.encodedSignal().empty())
        {
            painter.setPen(QPen(Qt::black, 1));
            painter.drawLine(zero + QPoint(0, - height / 2 - grid_margin), zero + QPoint(0, height / 2 + grid_margin));
        }

        zero.rx() = 20 + grid_margin;
        zero.ry() += height + graph_margin;  
    }
}

//void EncodingChart::drawFurie(QPainter& painter, const EncodedSignal& signal, const QRect& rect)
//{
//    // количество интервалов
//    const int N = signal.encodedSignal().size();

//    if (N == 0) return;

//    const float f0 = _frequency;

//    size_t i = 1;
//    std::vector<double> Ak(N + 1, 0.0), ak(N + 1, 0.0), bk(N + 1, 0.0);


//    for(level_t level : signal.encodedSignal())
//    {
//        ak[0] += level;

//        for (int k = 1; k <= N; k++)
//        {
//            ak[k] +=   (double)level * (sin(2 * M_PI * k * i / N) - sin(2 * M_PI * k * (i - 1) / N)) / (M_PI * k);
//            bk[k] += - (double)level * (cos(2 * M_PI * k * i / N) - cos(2 * M_PI * k * (i - 1) / N)) / (M_PI * k);
//        }

//        i += 1;
//    }

//    ak[0] /= N;
//    bk[0] = 0;

////    qDebug() << "Fourier: ";

//    for (int k = 1; k <= N; k++)
//    {
//        Ak[k] = sqrt(ak[k] * ak[k] + bk[k] * bk[k]);

//        if (Ak[k] >= 1e-2)
//        {
//            qDebug() << "k = " << k << " N = " << N << " f = " << f0 * k / N;
//            // break;
//        }
//    }

////    qDebug() << "End fourier";

////    const int min = std::max(0, _min);
////    const int max = std::min(N, _max);

////    qDebug() << "Frequency: ";
////    qDebug() << "f_min = " << f0 * min / N << " f_max = " << f0 * max / N;
////    qDebug() << "End of Frequency";

//    // Calculate spectrum

////    int k_min = 1;
////    int k_max = N;

////    const double barier = 0.3;

////    for (;; k_max--)
////    {
////        bool fail = false;
////        const double step = 1 / f0;

////        auto f = [&](double t) -> double
////        {
////            double r = ak[0];

////            for (int k = k_min; k <= k_max; k++)
////            {
////                r += ak[k] * cos(2 * M_PI * f0 * k * t / N) + bk[k] * sin(2 * M_PI * f0 * k * t / N);
////            }

////            return r;
////        };

////        for (size_t i = 0; i != _signal.encodedSignal().size(); i++)
////        {
////            const level_t curr = _signal.encodedSignal()[i];
////            const double f_t = f(step / 2 + i * step);

////            if (f_t > barier)
////            {
////                if (curr != 1)
////                {
////                    fail = true;
////                    break;
////                }
////            }
////            else if (f_t < -barier)
////            {
////                if (curr != -1)
////                {
////                    fail = true;
////                    break;
////                }
////            }
////            else
////            {
////                if (curr != 0)
////                {
////                    fail = true;
////                    break;
////                }
////            }
////        }

////        if (fail)
////        {
////            k_max++;
////            break;
////        }
////    }

////    for (;; k_min++)
////    {
////        bool fail = false;
////        const double step = 1 / f0;

////        auto f = [&](double t) -> double
////        {
////            double r = ak[0];

////            for (int k = k_min; k <= k_max; k++)
////            {
////                r += ak[k] * cos(2 * M_PI * f0 * k * t / N) + bk[k] * sin(2 * M_PI * f0 * k * t / N);
////            }

////            return r;
////        };

////        for (size_t i = 0; i != _signal.encodedSignal().size(); i++)
////        {
////            const level_t curr = _signal.encodedSignal()[i];
////            const double f_t = f(step / 2 + i * step);

////            if (f_t > barier)
////            {
////                if (curr != 1)
////                {
////                    fail = true;
////                    break;
////                }
////            }
////            else if (f_t < -barier)
////            {
////                if (curr != -1)
////                {
////                    fail = true;
////                    break;
////                }
////            }
////            else
////            {
////                if (curr != 0)
////                {
////                    fail = true;
////                    break;
////                }
////            }
////        }

////        if (fail)
////        {
////            k_min--;
////            break;
////        }
////    }

////    qDebug() << "Fourier analysis";
////    qDebug() << "k min = " << k_min << " N = " << N << " f = " << f0 * k_min / N;
////    qDebug() << "k max = " << k_max << " N = " << N << " f = " << f0 * k_max / N;
////    qDebug() << "End of Fourier analysis";

////    auto f = [&](double t) -> double
////    {
////        double r = ak[0];

////        for (int k = k_min; k <= k_max; k++)
////        {
////            r += ak[k] * cos(2 * M_PI * f0 * k * t / N) + bk[k] * sin(2 * M_PI * f0 * k * t / N);
////        }

////        return r;
////    };

//    QPoint zero = rect.topLeft();
//    const int width = rect.width();
//    const int height = rect.height();

//    painter.setPen(QPen(QColor(0xCC, 0x00, 0xCC), 2));

//    const double pixel_step = 1 / (width * f0);
////    double f_low = 100 * f0, f_high = 0;

////    double f_last = 0;
//    QPoint last = zero;

////    for (double t = 0; t < N / (1 * f0); t += pixel_step)
////    {
////        const double f_t = f(t);
////        const QPoint curr = zero + (QPointF(_mult[signal.type()] * f0 * width * t, -height * f_t / 2)).toPoint();

////        painter.drawLine(last, curr);
////        last = curr;
////    }

//    painter.setPen(QPen(QColor(0xCC, 0xCC, 0x00), 2));

//    auto ff = [&](double t) -> double
//    {
//        double r = ak[0];

//        for (int k = std::max(_min, 1); k <= _max; k++)
//        {
//            r += ak[k] * cos(2 * M_PI * f0 * k * t / N) + bk[k] * sin(2 * M_PI * f0 * k * t / N);
//        }

//        return r;
//    };


////    double f_low = 100 * f0, f_high = 0;

////    double f_last = 0;
//    last = zero;

//    for (double t = 0; t < N / (1 * f0); t += pixel_step)
//    {
//        const double f_t = ff(t);
//        const QPoint curr = zero + (QPointF(_mult[signal.type()] * f0 * width * t, -height * f_t / 2)).toPoint();

//        painter.drawLine(last, curr);
//        last = curr;
//    }


////    qDebug() << "min " << f_low << " max " << f_high;
//}
