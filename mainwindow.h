#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QPropertyAnimation>
#include <memory>
#include "Player.h"
#include "ComputerPlayer.h"
#include "QuestionGenerator.h"
#include "ScoreTracker.h"
#include "Difficulty.h"
#include "Question.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// One row in the cumulative match history
struct MatchRecord {
    int  roundNumber;
    QString winner;
    int  playerWins;
    int  computerWins;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(float tugPosition READ tugPositionF WRITE setTugPositionF)

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    float tugPositionF() const { return tugPositionF_; }
    void  setTugPositionF(float v);

private slots:
    void onAnswerSubmitted();
    void onTimerTick();

private:
    Ui::MainWindow* ui;

    // Game state
    std::unique_ptr<Player>         player1_;
    std::unique_ptr<ComputerPlayer> computer_;
    QuestionGenerator               questionGen_;
    ScoreTracker                    scoreTracker_;
    Difficulty                      difficulty_;
    Question                        currentQuestion_;
    int                             tugPosition_;
    float                           tugPositionF_;
    int                             timeLeft_;
    QTimer*                         timer_;
    QPropertyAnimation*             ropeAnim_;
    bool                            gameOver_;

    // Cumulative session stats (persist across Play Again)
    QString         playerName_;
    int             sessionPlayerWins_;
    int             sessionComputerWins_;
    int             sessionRound_;
    QList<MatchRecord> matchHistory_;

    // Game flow
    void showDifficultyDialog();
    void setupGame();
    void showNextQuestion();
    void resolveRound(double playerAnswer);
    void endGame();
    void updateTimerDisplay();
    void flashMessage(const QString& msg, const QColor& color);
    void animateRopeTo(int targetPosition);

    // Score panel
    void updateScorePanel();
    void refreshHistoryTable();
};
