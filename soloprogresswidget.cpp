#include "soloprogresswidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <cmath>

SoloProgressWidget::SoloProgressWidget(QWidget* parent)
    : QWidget(parent),
      correct_(0), total_(0), timeLeft_(60),
      playerName_("Player"),
      flashGreen_(0.0f), flashRed_(0.0f)
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void SoloProgressWidget::setStats(int correct, int total, int timeLeft) {
    correct_  = correct;
    total_    = total;
    timeLeft_ = timeLeft;
    update();
}

void SoloProgressWidget::setPlayerName(const QString& name) {
    playerName_ = name;
    update();
}

void SoloProgressWidget::flashCorrect() {
    flashGreen_ = 1.0f;
    QTimer* t = new QTimer(this);
    t->setInterval(50);
    connect(t, &QTimer::timeout, [this, t]() {
        flashGreen_ -= 0.1f;
        if (flashGreen_ <= 0.0f) { flashGreen_ = 0.0f; t->stop(); t->deleteLater(); }
        update();
    });
    t->start();
}

void SoloProgressWidget::flashWrong() {
    flashRed_ = 1.0f;
    QTimer* t = new QTimer(this);
    t->setInterval(50);
    connect(t, &QTimer::timeout, [this, t]() {
        flashRed_ -= 0.1f;
        if (flashRed_ <= 0.0f) { flashRed_ = 0.0f; t->stop(); t->deleteLater(); }
        update();
    });
    t->start();
}

QColor SoloProgressWidget::barColor() const {
    if (total_ == 0) return QColor(100, 100, 100);
    float acc = (float)correct_ / total_;
    if (acc >= 0.75f) return QColor(0,   200, 100);
    if (acc >= 0.50f) return QColor(220, 180,   0);
    if (acc >= 0.25f) return QColor(220, 100,   0);
    return QColor(220, 50, 50);
}

void SoloProgressWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    drawBackground(p);
    drawTimerRing(p);
    drawStats(p);
    drawProgressBar(p);
}

void SoloProgressWidget::drawBackground(QPainter& p) const {
    QLinearGradient bg(0, 0, 0, height());
    bg.setColorAt(0.0, QColor(10, 20, 50));
    bg.setColorAt(1.0, QColor(22, 33, 62));
    p.fillRect(rect(), bg);

    if (flashGreen_ > 0.0f) {
        QColor fc(0, 255, 100, static_cast<int>(flashGreen_ * 40));
        p.fillRect(rect(), fc);
    }
    if (flashRed_ > 0.0f) {
        QColor fc(255, 50, 50, static_cast<int>(flashRed_ * 40));
        p.fillRect(rect(), fc);
    }
}

void SoloProgressWidget::drawTimerRing(QPainter& p) const {
    int cx = width() / 2;
    int cy = 60;
    int r  = 48;

    // Background ring
    p.setPen(QPen(QColor(40, 40, 80), 8));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(QPoint(cx, cy), r, r);

    // Progress arc
    float fraction = timeLeft_ / 60.0f;
    int   spanAngle = static_cast<int>(fraction * 360 * 16);
    QColor ringColor = timeLeft_ > 20 ? QColor(0, 200, 100)
                     : timeLeft_ > 10 ? QColor(220, 180, 0)
                                      : QColor(220, 50, 50);
    p.setPen(QPen(ringColor, 8, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(cx - r, cy - r, r * 2, r * 2, 90 * 16, spanAngle);

    // Time text
    p.setFont(QFont("Arial", 16, QFont::Bold));
    p.setPen(ringColor);
    p.drawText(cx - 25, cy - 14, 50, 30,
               Qt::AlignHCenter | Qt::AlignVCenter,
               QString::number(timeLeft_));

    p.setFont(QFont("Arial", 8));
    p.setPen(QColor(150, 150, 150));
    p.drawText(cx - 25, cy + 12, 50, 16, Qt::AlignHCenter, "seconds");
}

void SoloProgressWidget::drawStats(QPainter& p) const {
    // Player name — sits below the timer ring with clear spacing
    int nameY = 118;
    p.setFont(QFont("Arial", 13, QFont::Bold));
    p.setPen(QColor(255, 215, 0));
    p.drawText(0, nameY, width(), 28, Qt::AlignHCenter,
               "🎯  " + playerName_);

    // Correct / Total counts — below the name
    int cx    = width() / 2;
    int statsY = nameY + 36;

    p.setFont(QFont("Arial", 22, QFont::Bold));
    p.setPen(QColor(0, 255, 153));
    p.drawText(cx - 160, statsY, 140, 40, Qt::AlignHCenter,
               QString::number(correct_));

    p.setFont(QFont("Arial", 14));
    p.setPen(QColor(150, 150, 150));
    p.drawText(cx - 20, statsY + 8, 40, 25, Qt::AlignHCenter, "/");

    p.setFont(QFont("Arial", 22, QFont::Bold));
    p.setPen(QColor(255, 107, 107));
    p.drawText(cx + 20, statsY, 140, 40, Qt::AlignHCenter,
               QString::number(total_));

    // Labels under counts
    p.setFont(QFont("Arial", 10));
    p.setPen(QColor(150, 150, 150));
    p.drawText(cx - 160, statsY + 42, 140, 20, Qt::AlignHCenter, "correct");
    p.drawText(cx + 20,  statsY + 42, 140, 20, Qt::AlignHCenter, "answered");

    // Wrong count
    int wrong = total_ - correct_;
    p.setFont(QFont("Arial", 11));
    p.setPen(QColor(255, 107, 107));
    p.drawText(0, statsY + 66, width(), 22, Qt::AlignHCenter,
               "❌  " + QString::number(wrong) + " wrong");
}

void SoloProgressWidget::drawProgressBar(QPainter& p) const {
    // Bar sits at the bottom of the widget with generous top margin from stats
    int barX = 40;
    int barY = height() - 60;   // anchored to bottom
    int barW = width() - 80;
    int barH = 36;
    int radius = 10;

    // Background track
    p.setBrush(QColor(30, 30, 60));
    p.setPen(QPen(QColor(60, 60, 100), 2));
    p.drawRoundedRect(barX, barY, barW, barH, radius, radius);

    // Fill
    float accuracy = total_ > 0 ? (float)correct_ / total_ : 0.0f;
    int   fillW    = static_cast<int>(accuracy * barW);

    if (fillW > 0) {
        QColor col = barColor();
        QLinearGradient grad(barX, barY, barX + fillW, barY);
        grad.setColorAt(0.0, col.lighter(120));
        grad.setColorAt(1.0, col);

        QPainterPath fillPath;
        fillPath.addRoundedRect(barX, barY, fillW, barH, radius, radius);
        p.fillPath(fillPath, grad);

        // Shine
        QLinearGradient shine(barX, barY, barX, barY + barH / 2);
        shine.setColorAt(0.0, QColor(255, 255, 255, 40));
        shine.setColorAt(1.0, QColor(255, 255, 255, 0));
        p.fillPath(fillPath, shine);
    }

    // Percentage text on bar
    int pct = total_ > 0 ? static_cast<int>(accuracy * 100) : 0;
    p.setFont(QFont("Arial", 13, QFont::Bold));
    p.setPen(QColor(255, 255, 255));
    p.drawText(barX, barY, barW, barH,
               Qt::AlignHCenter | Qt::AlignVCenter,
               QString::number(pct) + "% accuracy");

    // 0% / 100% labels below bar
    p.setFont(QFont("Arial", 9));
    p.setPen(QColor(150, 150, 150));
    p.drawText(barX,            barY + barH + 4, 30, 16, Qt::AlignLeft,  "0%");
    p.drawText(barX + barW - 30, barY + barH + 4, 30, 16, Qt::AlignRight, "100%");
}
