#include "tugofwarwidget.h"
#include <QPainterPath>
#include <cmath>

TugOfWarWidget::TugOfWarWidget(QWidget* parent)
    : QWidget(parent),
      position_(0),
      player1Name_("Player 1"),
      player2Name_("Player 2")
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void TugOfWarWidget::setPosition(int position) {
    position_ = qBound(-10, position, 10);
    update();
}

void TugOfWarWidget::setPlayerNames(const QString& player1, const QString& player2) {
    player1Name_ = player1;
    player2Name_ = player2;
    update();
}

// ── Layout ─────────────────────────────────────────────────────────────────
static Layout computeLayout(int w, int h, int position) {
    Layout l;
    l.centerX = w / 2;
    l.groundY = h - 80;          // where the green ground line is
    l.ropeY   = l.groundY - 80;  // rope held at this height above ground

    float shift = position * 5.0f;
    l.p2X = l.centerX - 200 + static_cast<int>(shift);
    l.p1X = l.centerX + 200 + static_cast<int>(shift);
    l.knotX   = l.centerX + position * 14.0f;
    l.p2HandX = l.p2X + 30.0f;
    l.p1HandX = l.p1X - 30.0f;
    return l;
}

// ── Paint ──────────────────────────────────────────────────────────────────
void TugOfWarWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    Layout l = computeLayout(width(), height(), position_);

    drawBackground(painter);
    drawGround(painter, l.groundY);
    drawRope(painter, l);

    float p1Lean = position_ >= 0 ?  0.18f :  0.18f + 0.02f * position_;
    float p2Lean = position_ <= 0 ? -0.18f : -0.18f + 0.02f * position_;

    drawFigureGlow(painter, l.p2X, l.groundY, QColor(255, 107, 107));
    drawFigureGlow(painter, l.p1X, l.groundY, QColor(0,   255, 153));

    drawFigure(painter, l.p2X, l.groundY, true,  p2Lean, QColor(255, 107, 107), l);
    drawFigure(painter, l.p1X, l.groundY, false, p1Lean, QColor(0,   255, 153), l);

    // Player name labels
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    painter.setPen(QColor(255, 107, 107));
    painter.drawText(l.p2X - 50, l.groundY + 14, 100, 25,
                     Qt::AlignHCenter, player2Name_);
    painter.setPen(QColor(0, 255, 153));
    painter.drawText(l.p1X - 50, l.groundY + 14, 100, 25,
                     Qt::AlignHCenter, player1Name_);

    // Status text
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(180, 180, 180));
    QString posText = position_ == 0 ? "Dead even!"
                    : position_ > 0  ? player1Name_ + " pulling ahead!"
                                     : player2Name_ + " pulling ahead!";
    painter.drawText(0, l.groundY + 42, width(), 20, Qt::AlignHCenter, posText);
}

// ── Background ─────────────────────────────────────────────────────────────
void TugOfWarWidget::drawBackground(QPainter& painter) const {
    QLinearGradient sky(0, 0, 0, height());
    sky.setColorAt(0.0, QColor(10,  20,  50));
    sky.setColorAt(1.0, QColor(22,  33,  62));
    painter.fillRect(rect(), sky);

    // Subtle crowd silhouettes in background
    painter.setBrush(QColor(18, 28, 55));
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < width(); i += 18) {
        int bh = 20 + (i * 7 + 11) % 25;
        painter.drawEllipse(i, height() - 130 - bh, 14, bh);
    }
}

// ── Ground ─────────────────────────────────────────────────────────────────
void TugOfWarWidget::drawGround(QPainter& painter, int groundY) const {
    QLinearGradient grass(0, groundY, 0, height());
    grass.setColorAt(0.0, QColor(45, 90,  39));
    grass.setColorAt(1.0, QColor(20, 50,  18));
    painter.fillRect(0, groundY, width(), height() - groundY, grass);

    painter.setPen(QPen(QColor(100, 200, 80), 2));
    painter.drawLine(0, groundY, width(), groundY);

    // Centre flag
    int cx = width() / 2;
    painter.setPen(QPen(QColor(255, 215, 0), 2));
    painter.drawLine(cx, groundY - 20, cx, groundY);
    painter.setBrush(QColor(255, 215, 0));
    painter.setPen(Qt::NoPen);
    QPointF flag[3] = {
        QPointF(cx,      groundY - 20),
        QPointF(cx + 12, groundY - 14),
        QPointF(cx,      groundY - 8)
    };
    painter.drawPolygon(flag, 3);
}

// ── Rope (single, clean) ───────────────────────────────────────────────────
void TugOfWarWidget::drawRope(QPainter& painter, const Layout& l) const {
    float droop = 10.0f + std::abs(position_) * 1.0f;
    int   ry    = l.ropeY;

    QPainterPath left;
    left.moveTo(l.p2HandX, ry);
    left.quadTo((l.p2HandX + l.knotX) / 2.0f, ry + droop, l.knotX, ry);

    QPainterPath right;
    right.moveTo(l.knotX, ry);
    right.quadTo((l.knotX + l.p1HandX) / 2.0f, ry + droop, l.p1HandX, ry);

    // Shadow
    painter.setPen(QPen(QColor(40, 20, 0), 7, Qt::SolidLine, Qt::RoundCap));
    painter.drawPath(left);
    painter.drawPath(right);

    // Main rope — same thickness as before, clean single line
    painter.setPen(QPen(QColor(200, 150, 12), 4, Qt::SolidLine, Qt::RoundCap));
    painter.drawPath(left);
    painter.drawPath(right);

    // Knot
    painter.setBrush(QColor(180, 120, 10));
    painter.setPen(QPen(QColor(100, 60, 0), 2));
    painter.drawEllipse(QPointF(l.knotX, ry), 13, 13);
    painter.setBrush(QColor(255, 215, 0));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(l.knotX, ry), 9, 9);
    painter.setBrush(QColor(255, 245, 180));
    painter.drawEllipse(QPointF(l.knotX - 3, ry - 4), 4, 4);
}

// ── Figure Glow ────────────────────────────────────────────────────────────
void TugOfWarWidget::drawFigureGlow(QPainter& painter, int x, int y,
                                     const QColor& color) const {
    QRadialGradient glow(x, y - 60, 60);
    QColor g = color;
    g.setAlpha(35);
    glow.setColorAt(0.0, g);
    g.setAlpha(0);
    glow.setColorAt(1.0, g);
    painter.setBrush(glow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(x, y - 60), 60, 75);
}

// ── Silhouette Figure ──────────────────────────────────────────────────────
// x, y = base of feet ON the ground line
void TugOfWarWidget::drawFigure(QPainter& painter, int x, int y,
                                 bool facingRight, float leanAngle,
                                 const QColor& color, const Layout& l) const {
    // leanAngle shifts the upper body horizontally while feet stay planted
    int lean = static_cast<int>(leanAngle * 40.0f);

    // All Y coords are ABOVE the ground (y), so subtract from y
    int footY     = y;        // feet ON the ground
    int kneeY     = y - 28;
    int hipY      = y - 50;
    int shoulderY = y - 85;
    int headCY    = y - 108;

    // X coords shift with lean from hip upward
    int hipX      = x  + lean / 3;
    int shoulderX = x  + lean;
    int headX     = x  + lean;

    // Rope hand world position
    int handX = facingRight ? static_cast<int>(l.p2HandX)
                            : static_cast<int>(l.p1HandX);
    int handY = l.ropeY;

    int backDir = facingRight ? -1 : 1;

    // ── Torso ──
    QPainterPath torso;
    torso.moveTo(shoulderX - 16, shoulderY);
    torso.lineTo(shoulderX + 16, shoulderY);
    torso.lineTo(hipX      + 11, hipY);
    torso.lineTo(hipX      - 11, hipY);
    torso.closeSubpath();
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawPath(torso);

    // ── Pelvis ──
    painter.drawEllipse(QPointF(hipX, hipY + 4), 11, 7);

    // ── Upper legs ──
    QPen legPen(color, 10, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(legPen);
    painter.drawLine(hipX, hipY + 8, x - 9,  kneeY);  // left upper leg
    painter.drawLine(hipX, hipY + 8, x + 9,  kneeY);  // right upper leg

    // ── Lower legs ──
    QPen lowerLegPen(color, 8, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(lowerLegPen);
    painter.drawLine(x - 9, kneeY, x - 7,  footY);
    painter.drawLine(x + 9, kneeY, x + 7,  footY);

    // ── Feet ──
    int fd = facingRight ? 1 : -1;
    QPen footPen(color, 6, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(footPen);
    painter.drawLine(x - 7, footY, x - 7 + fd * 10, footY);
    painter.drawLine(x + 7, footY, x + 7 + fd * 10, footY);

    // ── Rope arm ──
    QPen armPen(color, 9, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(armPen);
    painter.drawLine(shoulderX, shoulderY, handX, handY);

    // ── Back arm ──
    painter.setPen(QPen(color, 8, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(shoulderX, shoulderY,
                     shoulderX + backDir * 20, shoulderY + 16);

    // ── Head ──
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(headX, headCY), 14, 16);

    // Hair bump
    painter.drawEllipse(QPoint(headX - 3, headCY - 13), 9, 6);

    // Face highlight
    QColor fh = color.lighter(140);
    fh.setAlpha(70);
    painter.setBrush(fh);
    int fd2 = facingRight ? 4 : -4;
    painter.drawEllipse(QPoint(headX + fd2, headCY - 2), 7, 9);

    // Eye
    painter.setBrush(QColor(255, 255, 255));
    int eyeX = facingRight ? headX + 7 : headX - 7;
    painter.drawEllipse(QPoint(eyeX, headCY - 1), 3, 3);
    painter.setBrush(QColor(20, 20, 40));
    int pupilX = facingRight ? eyeX + 1 : eyeX - 1;
    painter.drawEllipse(QPoint(pupilX, headCY - 1), 1, 1);
}
