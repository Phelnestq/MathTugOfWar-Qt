#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tugofwarwidget.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QTableWidget>
#include <QHeaderView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , difficulty_(Difficulty::PRIMARY_EASY)
    , currentQuestion_("", 0.0, Difficulty::PRIMARY_EASY)
    , tugPosition_(0)
    , tugPositionF_(0.0f)
    , timeLeft_(10)
    , gameOver_(false)
    , playerName_("Player")
    , sessionPlayerWins_(0)
    , sessionComputerWins_(0)
    , sessionRound_(0)
{
    ui->setupUi(this);
    setWindowTitle("Math Tug of War");

    timer_    = new QTimer(this);
    ropeAnim_ = new QPropertyAnimation(this, "tugPosition", this);
    ropeAnim_->setDuration(600);
    ropeAnim_->setEasingCurve(QEasingCurve::OutCubic);

    connect(timer_,          &QTimer::timeout,
            this,            &MainWindow::onTimerTick);
    connect(ui->inputAnswer, &QLineEdit::returnPressed,
            this,            &MainWindow::onAnswerSubmitted);

    showDifficultyDialog();
}

MainWindow::~MainWindow() { delete ui; }

// ── Animated rope property ─────────────────────────────────────────────────
void MainWindow::setTugPositionF(float v) {
    tugPositionF_ = v;
    TugOfWarWidget* tow = findChild<TugOfWarWidget*>("tugOfWarWidget");
    if (tow) tow->setPosition(static_cast<int>(v));
}

// ── Difficulty + name dialog ───────────────────────────────────────────────
void MainWindow::showDifficultyDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Math Tug of War");
    dialog.setFixedSize(520, 440);
    dialog.setStyleSheet(
        "QDialog { background-color: #1a1a2e; }"
        "QLabel  { color: #FFD700; }"
        "QLineEdit {"
        "  background-color: #2a2a4a; color: #FFFFFF;"
        "  border: 2px solid #FFD700; border-radius: 6px;"
        "  padding: 6px; font-size: 14px; }"
        "QPushButton {"
        "  background-color: #2a2a4a; color: #FFFFFF;"
        "  border: 2px solid #FFD700; border-radius: 8px;"
        "  padding: 10px 16px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover   { background-color: #FFD700; color: #1a1a2e; }"
        "QPushButton:checked { background-color: #FFD700; color: #1a1a2e; }"
    );

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setSpacing(12);
    layout->setContentsMargins(30, 20, 30, 20);

    // Title
    QLabel* title = new QLabel("🧮  MATH TUG OF WAR", &dialog);
    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #FFD700;");
    layout->addWidget(title);

    // Player name entry
    QLabel* nameLabel = new QLabel("Enter your name:", &dialog);
    nameLabel->setStyleSheet("font-size: 13px; color: #AAAAAA;");
    layout->addWidget(nameLabel);

    QLineEdit* nameInput = new QLineEdit(&dialog);
    nameInput->setPlaceholderText("Your name...");
    nameInput->setText(playerName_);  // pre-fill if playing again
    layout->addWidget(nameInput);

    // Difficulty label
    QLabel* sub = new QLabel("Select difficulty:", &dialog);
    sub->setStyleSheet("font-size: 13px; color: #AAAAAA;");
    layout->addWidget(sub);

    // Difficulty buttons
    struct Level { QString label; Difficulty diff; };
    QList<Level> levels = {
        { "🟢  Primary Easy    —  Age 5–8   (KS1/KS2)",  Difficulty::PRIMARY_EASY   },
        { "🟡  Primary Hard    —  Age 9–11  (KS2)",       Difficulty::PRIMARY_HARD   },
        { "🟠  Secondary Easy  —  Age 11–14 (KS3)",       Difficulty::SECONDARY_EASY },
        { "🔴  Secondary Hard  —  Age 14–16 (GCSE)",      Difficulty::SECONDARY_HARD },
    };

    QButtonGroup* group = new QButtonGroup(&dialog);
    group->setExclusive(true);
    QPushButton* firstBtn = nullptr;

    for (const auto& lvl : levels) {
        QPushButton* btn = new QPushButton(lvl.label, &dialog);
        btn->setCheckable(true);
        btn->setProperty("diff", static_cast<int>(lvl.diff));
        group->addButton(btn);
        layout->addWidget(btn);
        if (!firstBtn) { firstBtn = btn; btn->setChecked(true); }
    }

    // Start button
    QPushButton* startBtn = new QPushButton("▶  Start Game", &dialog);
    startBtn->setStyleSheet(
        "QPushButton { background-color: #00AA66; color: #FFFFFF;"
        "  border: 2px solid #00FF99; border-radius: 8px;"
        "  padding: 10px; font-size: 15px; font-weight: bold; }"
        "QPushButton:hover { background-color: #00FF99; color: #1a1a2e; }"
    );
    layout->addWidget(startBtn);

    connect(startBtn, &QPushButton::clicked, [&]() {
        QString name = nameInput->text().trimmed();
        playerName_  = name.isEmpty() ? "Player" : name;
        QAbstractButton* checked = group->checkedButton();
        if (checked)
            difficulty_ = static_cast<Difficulty>(checked->property("diff").toInt());
        dialog.accept();
    });

    dialog.exec();
    setupGame();
}

// ── Setup ──────────────────────────────────────────────────────────────────
void MainWindow::setupGame() {
    srand(static_cast<unsigned int>(time(nullptr)));

    player1_  = std::make_unique<Player>(playerName_.toStdString());
    computer_ = std::make_unique<ComputerPlayer>("Computer", difficulty_);

    tugPosition_  = 0;
    tugPositionF_ = 0.0f;
    gameOver_     = false;

    TugOfWarWidget* tow = findChild<TugOfWarWidget*>("tugOfWarWidget");
    if (tow) {
        tow->setPlayerNames(
            QString::fromStdString(player1_->getName()),
            QString::fromStdString(computer_->getName()));
        tow->setPosition(0);
    }

    ui->labelPlayer1->setText(QString::fromStdString(player1_->getName()));
    ui->labelPlayer2->setText(QString::fromStdString(computer_->getName()));

    updateScorePanel();
    showNextQuestion();
}

// ── Rope animation ─────────────────────────────────────────────────────────
void MainWindow::animateRopeTo(int targetPosition) {
    ropeAnim_->stop();
    ropeAnim_->setStartValue(tugPositionF_);
    ropeAnim_->setEndValue(static_cast<float>(targetPosition));
    ropeAnim_->start();
}

// ── Question display ───────────────────────────────────────────────────────
void MainWindow::showNextQuestion() {
    if (gameOver_) return;
    currentQuestion_ = questionGen_.generate(difficulty_);
    ui->labelQuestion->setText("❓  " + QString::fromStdString(
                                   currentQuestion_.getProblemText()));
    ui->inputAnswer->clear();
    ui->inputAnswer->setFocus();
    timeLeft_ = 10;
    updateTimerDisplay();
    timer_->start(1000);
}

// ── Timer ──────────────────────────────────────────────────────────────────
void MainWindow::onTimerTick() {
    timeLeft_--;
    updateTimerDisplay();

    if (timeLeft_ <= 0) {
        timer_->stop();
        flashMessage("⏱ Time's up! Computer pulls!", QColor(255, 100, 100));
        tugPosition_ = qBound(-10, tugPosition_ - 2, 10);
        animateRopeTo(tugPosition_);

        if (tugPosition_ <= -10) {
            QTimer::singleShot(700, this, &MainWindow::endGame);
        } else {
            QTimer::singleShot(1500, this, &MainWindow::showNextQuestion);
        }
    }
}

void MainWindow::updateTimerDisplay() {
    ui->labelTimer->setText("⏱  " + QString::number(timeLeft_) + "s");
    if (timeLeft_ > 5)
        ui->labelTimer->setStyleSheet(
            "color: #FFD700; font-size: 18px; font-weight: bold;");
    else if (timeLeft_ > 2)
        ui->labelTimer->setStyleSheet(
            "color: #FF8C00; font-size: 18px; font-weight: bold;");
    else
        ui->labelTimer->setStyleSheet(
            "color: #FF2222; font-size: 20px; font-weight: bold;");
}

// ── Answer submission ──────────────────────────────────────────────────────
void MainWindow::onAnswerSubmitted() {
    if (gameOver_) return;
    QString input = ui->inputAnswer->text().trimmed();
    if (input.isEmpty()) return;
    bool ok = false;
    double playerAnswer = input.toDouble(&ok);
    if (!ok) { ui->inputAnswer->clear(); return; }
    timer_->stop();
    resolveRound(playerAnswer);
}

// ── Round resolution ───────────────────────────────────────────────────────
void MainWindow::resolveRound(double playerAnswer) {
    bool   playerCorrect = currentQuestion_.checkAnswer(playerAnswer);
    double compAnswer    = computer_->simulateAnswer(currentQuestion_);
    bool   compCorrect   = currentQuestion_.checkAnswer(compAnswer);

    if (playerCorrect && compCorrect) {
        tugPosition_ += 1;
        flashMessage("✅ Both correct — you were faster!", QColor(0, 255, 153));
    } else if (playerCorrect && !compCorrect) {
        tugPosition_ += 2;
        flashMessage("✅ Correct! Computer was wrong. Strong pull!", QColor(0, 255, 153));
    } else if (!playerCorrect && compCorrect) {
        tugPosition_ -= 2;
        flashMessage("❌ Wrong! Answer was " +
                     QString::number(currentQuestion_.getAnswer()) +
                     ". Computer pulls!", QColor(255, 107, 107));
    } else {
        flashMessage("❌ Both wrong! Answer was: " +
                     QString::number(currentQuestion_.getAnswer()),
                     QColor(255, 200, 0));
    }

    tugPosition_ = qBound(-10, tugPosition_, 10);
    animateRopeTo(tugPosition_);

    if (tugPosition_ >= 10 || tugPosition_ <= -10) {
        QTimer::singleShot(700, this, &MainWindow::endGame);
    } else {
        QTimer::singleShot(1500, this, &MainWindow::showNextQuestion);
    }
}

// ── End game ───────────────────────────────────────────────────────────────
void MainWindow::endGame() {
    gameOver_ = true;
    timer_->stop();

    // Record result in session history
    sessionRound_++;
    bool playerWon = tugPosition_ >= 10;
    if (playerWon) sessionPlayerWins_++;
    else           sessionComputerWins_++;

    QString winner = playerWon
                   ? QString::fromStdString(player1_->getName())
                   : QString::fromStdString(computer_->getName());

    matchHistory_.append({ sessionRound_, winner,
                           sessionPlayerWins_, sessionComputerWins_ });
    updateScorePanel();
    refreshHistoryTable();

    // Game over dialog
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Game Over");
    msgBox.setText("🏆 " + winner + " wins!");
    msgBox.setInformativeText(
        "Session score:  " + playerName_ + " " +
        QString::number(sessionPlayerWins_) + "  —  Computer " +
        QString::number(sessionComputerWins_) +
        "\n\nPlay again?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.addButton("New Session", QMessageBox::ResetRole);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #1a1a2e; }"
        "QMessageBox QLabel { color: #FFD700; font-size: 15px; font-weight: bold; }"
        "QPushButton { background-color: #2a2a4a; color: #FFFFFF;"
        "  border: 2px solid #FFD700; border-radius: 6px;"
        "  padding: 6px 18px; font-size: 13px; }"
        "QPushButton:hover { background-color: #FFD700; color: #1a1a2e; }"
    );

    int reply = msgBox.exec();
    if (reply == QMessageBox::Yes) {
        // Play again — same name/difficulty, keep session scores
        setupGame();
    } else if (reply == QMessageBox::ResetRole) {
        // New session — reset all cumulative stats
        sessionPlayerWins_  = 0;
        sessionComputerWins_ = 0;
        sessionRound_       = 0;
        matchHistory_.clear();
        updateScorePanel();
        refreshHistoryTable();
        showDifficultyDialog();
    } else {
        close();
    }
}

// ── Flash message ──────────────────────────────────────────────────────────
void MainWindow::flashMessage(const QString& msg, const QColor& color) {
    ui->labelQuestion->setText(msg);
    ui->labelQuestion->setStyleSheet(
        "color: " + color.name() + "; font-size: 16px; font-weight: bold;");
    QTimer::singleShot(1400, this, [this]() {
        ui->labelQuestion->setStyleSheet(
            "color: #FFFFFF; font-size: 20px; font-weight: bold;");
    });
}

// ── Score panel update ─────────────────────────────────────────────────────
void MainWindow::updateScorePanel() {
    ui->labelSessionPlayer->setText(playerName_ + ":  " +
                                    QString::number(sessionPlayerWins_));
    ui->labelSessionComputer->setText("Computer:  " +
                                      QString::number(sessionComputerWins_));
    ui->labelSessionRound->setText("Round:  " +
                                   QString::number(sessionRound_));
}

// ── History table refresh ──────────────────────────────────────────────────
void MainWindow::refreshHistoryTable() {
    QTableWidget* table = ui->tableHistory;
    table->setRowCount(matchHistory_.size());

    for (int i = 0; i < matchHistory_.size(); ++i) {
        const MatchRecord& r = matchHistory_[i];

        auto* roundItem    = new QTableWidgetItem(QString::number(r.roundNumber));
        auto* winnerItem   = new QTableWidgetItem(r.winner);
        auto* scoreItem    = new QTableWidgetItem(
            QString::number(r.playerWins) + " — " +
            QString::number(r.computerWins));

        roundItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        winnerItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        scoreItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        // Highlight winner row
        QColor rowColor = (r.winner == playerName_)
                        ? QColor(0, 80, 40)
                        : QColor(80, 20, 20);
        roundItem->setBackground(rowColor);
        winnerItem->setBackground(rowColor);
        scoreItem->setBackground(rowColor);

        table->setItem(i, 0, roundItem);
        table->setItem(i, 1, winnerItem);
        table->setItem(i, 2, scoreItem);
    }
    // Scroll to latest
    if (!matchHistory_.isEmpty())
        table->scrollToBottom();
}
