#pragma once
#include <QWidget>
#include <QString>

class SoloProgressWidget : public QWidget {
    Q_OBJECT

public:
    explicit SoloProgressWidget(QWidget* parent = nullptr);

    void setStats(int correct, int total, int timeLeft);
    void setPlayerName(const QString& name);
    void flashCorrect();
    void flashWrong();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int     correct_;
    int     total_;
    int     timeLeft_;
    QString playerName_;
    float   flashGreen_;  // 0.0 to 1.0 flash intensity
    float   flashRed_;

    QColor barColor() const;
    void drawBackground(QPainter& p) const;
    void drawProgressBar(QPainter& p) const;
    void drawStats(QPainter& p) const;
    void drawTimerRing(QPainter& p) const;
};
