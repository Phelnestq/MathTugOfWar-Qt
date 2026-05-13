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

enum class GameMode { VsComputer, Solo };

struct MatchRecord {
    int     roundNumber;
    QString winner;       // player name for vs, "Solo" for practice
    int     playerWins;
    int     computerWins;
    QString soloScore;    // e.g. "7/10" for solo mode
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
    void onSoloTimerTick();

private:
    Ui::MainWindow* ui;

    // Game state
    std::unique_ptr<Player>         player1_;
    std::unique_ptr<ComputerPlayer> computer_;
    QuestionGenerator               questionGen_;
    ScoreTracker                    scoreTracker_;
    Difficulty                      difficulty_;
    GameMode                        gameMode_;
    Question                        currentQuestion_;
    int                             tugPosition_;
    float                           tugPositionF_;
    int                             timeLeft_;       // per-question timer
    int                             soloTimeLeft_;   // 60s session timer
    int                             soloCorrect_;
    int                             soloTotal_;
    QTimer*                         timer_;          // per-question
    QTimer*                         soloTimer_;      // 60s countdown
    QPropertyAnimation*             ropeAnim_;
    bool                            gameOver_;

    // Session stats
    QString            playerName_;
    int                sessionPlayerWins_;
    int                sessionComputerWins_;
    int                sessionRound_;
    QList<MatchRecord> matchHistory_;

    // Game flow
    void showDifficultyDialog();
    void setupGame();
    void setupSoloGame();
    void setupVsGame();
    void showNextQuestion();
    void resolveRound(double playerAnswer);
    void resolveSoloRound(double playerAnswer);
    void endGame();
    void endSoloGame();
    void updateTimerDisplay();
    void flashMessage(const QString& msg, const QColor& color);
    void animateRopeTo(int targetPosition);
    void updateScorePanel();
    void refreshHistoryTable();
    void resizeEvent(QResizeEvent* event) override;
    void switchToSoloMode();
    void switchToVsMode();
};
