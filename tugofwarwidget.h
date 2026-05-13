#pragma once
#include <QWidget>
#include <QString>
#include <QPainter>

struct Layout {
    int centerX;
    int groundY;
    int ropeY;
    int p1X;
    int p2X;
    float knotX;
    float p1HandX;
    float p2HandX;
};

class TugOfWarWidget : public QWidget {
    Q_OBJECT

public:
    explicit TugOfWarWidget(QWidget* parent = nullptr);

    void setPosition(int position);
    void setPlayerNames(const QString& player1, const QString& player2);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int position_;
    QString player1Name_;
    QString player2Name_;

    void drawBackground(QPainter& painter) const;
    void drawGround(QPainter& painter, int groundY) const;
    void drawRope(QPainter& painter, const Layout& l) const;
    void drawFigureGlow(QPainter& painter, int x, int y,
                        const QColor& color) const;
    void drawFigure(QPainter& painter, int x, int y,
                    bool facingRight, float leanAngle,
                    const QColor& color, const Layout& l) const;
};
