#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tugofwarwidget.h"
#include "soloprogresswidget.h"
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
    , gameMode_(GameMode::VsComputer)
    , currentQuestion_("", 0.0, Difficulty::PRIMARY_EASY)
    , tugPosition_(0), tugPositionF_(0.0f)
    , timeLeft_(10), soloTimeLeft_(60)
    , soloCorrect_(0), soloTotal_(0)
    , gameOver_(false)
    , playerName_("Player")
    , sessionPlayerWins_(0), sessionComputerWins_(0), sessionRound_(0)
{
    ui->setupUi(this);
    setWindowTitle("Math Tug of War");

    timer_     = new QTimer(this);
    soloTimer_ = new QTimer(this);
    ropeAnim_  = new QPropertyAnimation(this, "tugPosition", this);
    ropeAnim_->setDuration(600);
    ropeAnim_->setEasingCurve(QEasingCurve::OutCubic);

    connect(timer_,     &QTimer::timeout, this, &MainWindow::onTimerTick);
    connect(soloTimer_, &QTimer::timeout, this, &MainWindow::onSoloTimerTick);
    connect(ui->inputAnswer, &QLineEdit::returnPressed,
            this, &MainWindow::onAnswerSubmitted);

    showDifficultyDialog();
}

MainWindow::~MainWindow() { delete ui; }

// ── Animated rope property ─────────────────────────────────────────────────
void MainWindow::setTugPositionF(float v) {
    tugPositionF_ = v;
    TugOfWarWidget* tow = findChild<TugOfWarWidget*>("tugOfWarWidget");
    if (tow) tow->setPosition(static_cast<int>(v));
}

// ── Mode switching ─────────────────────────────────────────────────────────
void MainWindow::switchToSoloMode() {
    TugOfWarWidget*    tow  = findChild<TugOfWarWidget*>("tugOfWarWidget");
    SoloProgressWidget* spw = findChild<SoloProgressWidget*>("soloProgressWidget");
    if (tow)  tow->hide();
    if (spw)  spw->show();
    // Hide player 2 label in solo
    ui->labelPlayer2->hide();
}

void MainWindow::switchToVsMode() {
    TugOfWarWidget*    tow  = findChild<TugOfWarWidget*>("tugOfWarWidget");
    SoloProgressWidget* spw = findChild<SoloProgressWidget*>("soloProgressWidget");
    if (tow)  tow->show();
    if (spw)  spw->hide();
    ui->labelPlayer2->show();
}

// ── Difficulty + mode + name dialog ───────────────────────────────────────
void MainWindow::showDifficultyDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Math Tug of War");
    dialog.setFixedSize(520, 500);
    dialog.setStyleSheet(
        "QDialog { background-color: #1a1a2e; }"
        "QLabel  { color: #FFD700; }"
        "QLineEdit { background-color: #2a2a4a; color: #FFFFFF;"
        "  border: 2px solid #FFD700; border-radius: 6px; padding: 6px; font-size: 14px; }"
        "QPushButton { background-color: #2a2a4a; color: #FFFFFF;"
        "  border: 2px solid #FFD700; border-radius: 8px;"
        "  padding: 0px 16px; min-height: 40px; font-size: 13px; font-weight: bold;"
        "  text-align: center; }"
        "QPushButton:hover   { background-color: #FFD700; color: #1a1a2e; }"
        "QPushButton:checked { background-color: #FFD700; color: #1a1a2e; }"
    );

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->setSpacing(10);
    layout->setContentsMargins(30, 18, 30, 18);

    // Title
    QLabel* title = new QLabel("🧮  MATH TUG OF WAR", &dialog);
    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("font-size: 22px; font-weight: bold; color: #FFD700;");
    layout->addWidget(title);

    // Player name
    QLabel* nameLabel = new QLabel("Enter your name:", &dialog);
    nameLabel->setStyleSheet("font-size: 12px; color: #AAAAAA;");
    layout->addWidget(nameLabel);

    QLineEdit* nameInput = new QLineEdit(&dialog);
    nameInput->setPlaceholderText("Your name...");
    nameInput->setText(playerName_);
    layout->addWidget(nameInput);

    // Game mode selection
    QLabel* modeLabel = new QLabel("Select game mode:", &dialog);
    modeLabel->setStyleSheet("font-size: 12px; color: #AAAAAA;");
    layout->addWidget(modeLabel);

    QHBoxLayout* modeLayout = new QHBoxLayout();
    QPushButton* vsBtn   = new QPushButton("⚔️  vs Computer", &dialog);
    QPushButton* soloBtn = new QPushButton("🎯  Solo Practice", &dialog);
    vsBtn->setCheckable(true);
    soloBtn->setCheckable(true);

    QButtonGroup* modeGroup = new QButtonGroup(&dialog);
    modeGroup->setExclusive(true);
    modeGroup->addButton(vsBtn);
    modeGroup->addButton(soloBtn);

    if (gameMode_ == GameMode::VsComputer) vsBtn->setChecked(true);
    else                                   soloBtn->setChecked(true);

    modeLayout->addWidget(vsBtn);
    modeLayout->addWidget(soloBtn);
    layout->addLayout(modeLayout);

    // Solo info label
    QLabel* soloInfo = new QLabel("Solo: answer as many as you can in 60 seconds", &dialog);
    soloInfo->setAlignment(Qt::AlignHCenter);
    soloInfo->setStyleSheet("font-size: 11px; color: #888888;");
    layout->addWidget(soloInfo);

    // Difficulty
    QLabel* diffLabel = new QLabel("Select difficulty:", &dialog);
    diffLabel->setStyleSheet("font-size: 12px; color: #AAAAAA;");
    layout->addWidget(diffLabel);

    struct Level { QString label; Difficulty diff; };
    QList<Level> levels = {
        { "🟢  Primary Easy    —  Age 5–8   (KS1/KS2)",  Difficulty::PRIMARY_EASY   },
        { "🟡  Primary Hard    —  Age 9–11  (KS2)",       Difficulty::PRIMARY_HARD   },
        { "🟠  Secondary Easy  —  Age 11–14 (KS3)",       Difficulty::SECONDARY_EASY },
        { "🔴  Secondary Hard  —  Age 14–16 (GCSE)",      Difficulty::SECONDARY_HARD },
    };

    QButtonGroup* diffGroup = new QButtonGroup(&dialog);
    diffGroup->setExclusive(true);
    QPushButton* firstBtn = nullptr;

    for (const auto& lvl : levels) {
        QPushButton* btn = new QPushButton(lvl.label, &dialog);
        btn->setCheckable(true);
        btn->setProperty("diff", static_cast<int>(lvl.diff));
        diffGroup->addButton(btn);
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
        gameMode_    = vsBtn->isChecked() ? GameMode::VsComputer : GameMode::Solo;
        QAbstractButton* checked = diffGroup->checkedButton();
        if (checked)
            difficulty_ = static_cast<Difficulty>(checked->property("diff").toInt());
        dialog.accept();
    });

    dialog.exec();
    setupGame();
}

// ── Setup dispatcher ───────────────────────────────────────────────────────
void MainWindow::setupGame() {
    srand(static_cast<unsigned int>(time(nullptr)));
    gameOver_ = false;

    if (gameMode_ == GameMode::Solo) setupSoloGame();
    else                             setupVsGame();

    updateScorePanel();
    showNextQuestion();
}

// ── VS Computer setup ──────────────────────────────────────────────────────
void MainWindow::setupVsGame() {
    player1_  = std::make_unique<Player>(playerName_.toStdString());
    computer_ = std::make_unique<ComputerPlayer>("Computer", difficulty_);

    tugPosition_  = 0;
    tugPositionF_ = 0.0f;

    switchToVsMode();

    TugOfWarWidget* tow = findChild<TugOfWarWidget*>("tugOfWarWidget");
    if (tow) {
        tow->setPlayerNames(
            QString::fromStdString(player1_->getName()),
            QString::fromStdString(computer_->getName()));
        tow->setPosition(0);
    }

    ui->labelPlayer1->setText(QString::fromStdString(player1_->getName()));
    ui->labelPlayer2->setText("Computer");
}

// ── Solo setup ─────────────────────────────────────────────────────────────
void MainWindow::setupSoloGame() {
    player1_      = std::make_unique<Player>(playerName_.toStdString());
    soloCorrect_  = 0;
    soloTotal_    = 0;
    soloTimeLeft_ = 60;

    switchToSoloMode();

    SoloProgressWidget* spw = findChild<SoloProgressWidget*>("soloProgressWidget");
    if (spw) {
        spw->setPlayerName(playerName_);
        spw->setStats(0, 0, 60);
    }

    ui->labelPlayer1->setText(playerName_);
    soloTimer_->start(1000);
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

// ── Per-question timer ─────────────────────────────────────────────────────
void MainWindow::onTimerTick() {
    timeLeft_--;
    updateTimerDisplay();

    if (timeLeft_ <= 0) {
        timer_->stop();

        if (gameMode_ == GameMode::Solo) {
            // Time up on question in solo — counts as wrong
            soloTotal_++;
            SoloProgressWidget* spw = findChild<SoloProgressWidget*>("soloProgressWidget");
            if (spw) {
                spw->flashWrong();
                spw->setStats(soloCorrect_, soloTotal_, soloTimeLeft_);
            }
            flashMessage("⏱ Time's up! Next question...", QColor(255, 100, 100));
            QTimer::singleShot(1000, this, &MainWindow::showNextQuestion);
        } else {
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
}

// ── 60s solo session timer ─────────────────────────────────────────────────
void MainWindow::onSoloTimerTick() {
    soloTimeLeft_--;
    SoloProgressWidget* spw = findChild<SoloProgressWidget*>("soloProgressWidget");
    if (spw) spw->setStats(soloCorrect_, soloTotal_, soloTimeLeft_);

    if (soloTimeLeft_ <= 0) {
        soloTimer_->stop();
        timer_->stop();
        endSoloGame();
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

    if (gameMode_ == GameMode::Solo) resolveSoloRound(playerAnswer);
    else                             resolveRound(playerAnswer);
}

// ── Solo round resolution ──────────────────────────────────────────────────
void MainWindow::resolveSoloRound(double playerAnswer) {
    bool correct = currentQuestion_.checkAnswer(playerAnswer);
    soloTotal_++;
    if (correct) soloCorrect_++;

    SoloProgressWidget* spw = findChild<SoloProgressWidget*>("soloProgressWidget");
    if (spw) {
        if (correct) spw->flashCorrect();
        else         spw->flashWrong();
        spw->setStats(soloCorrect_, soloTotal_, soloTimeLeft_);
    }

    if (correct) {
        flashMessage("✅ Correct! Keep going!", QColor(0, 255, 153));
    } else {
        flashMessage("❌ Wrong! Answer was: " +
                     QString::number(currentQuestion_.getAnswer()),
                     QColor(255, 107, 107));
    }

    QTimer::singleShot(1000, this, &MainWindow::showNextQuestion);
}

// ── VS round resolution ────────────────────────────────────────────────────
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

// ── End solo game ──────────────────────────────────────────────────────────
void MainWindow::endSoloGame() {
    gameOver_ = true;
    sessionRound_++;

    int pct = soloTotal_ > 0 ? soloCorrect_ * 100 / soloTotal_ : 0;
    QString soloScoreStr = QString::number(soloCorrect_) + "/" +
                           QString::number(soloTotal_) + " (" +
                           QString::number(pct) + "%)";

    matchHistory_.append({ sessionRound_, "Solo", 0, 0, soloScoreStr });
    updateScorePanel();
    refreshHistoryTable();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Time's Up!");
    msgBox.setText("⏱ 60 seconds over!");
    msgBox.setInformativeText(
        playerName_ + " scored " + soloScoreStr +
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
        setupGame();
    } else if (reply == QMessageBox::ResetRole) {
        sessionPlayerWins_   = 0;
        sessionComputerWins_ = 0;
        sessionRound_        = 0;
        matchHistory_.clear();
        updateScorePanel();
        refreshHistoryTable();
        showDifficultyDialog();
    } else {
        close();
    }
}

// ── End VS game ────────────────────────────────────────────────────────────
void MainWindow::endGame() {
    gameOver_ = true;
    timer_->stop();

    sessionRound_++;
    bool playerWon = tugPosition_ >= 10;
    if (playerWon) sessionPlayerWins_++;
    else           sessionComputerWins_++;

    QString winner = playerWon
                   ? QString::fromStdString(player1_->getName())
                   : QString::fromStdString(computer_->getName());

    matchHistory_.append({ sessionRound_, winner,
                           sessionPlayerWins_, sessionComputerWins_, "" });
    updateScorePanel();
    refreshHistoryTable();

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Game Over");
    msgBox.setText("🏆 " + winner + " wins!");
    msgBox.setInformativeText(
        "Session:  " + playerName_ + " " +
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
        setupGame();
    } else if (reply == QMessageBox::ResetRole) {
        sessionPlayerWins_   = 0;
        sessionComputerWins_ = 0;
        sessionRound_        = 0;
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
    QTimer::singleShot(900, this, [this]() {
        ui->labelQuestion->setStyleSheet(
            "color: #FFFFFF; font-size: 20px; font-weight: bold;");
    });
}

// ── Score panel ────────────────────────────────────────────────────────────
void MainWindow::updateScorePanel() {
    ui->labelSessionPlayer->setText(playerName_ + ":  " +
                                    QString::number(sessionPlayerWins_));
    ui->labelSessionComputer->setText("Computer:  " +
                                      QString::number(sessionComputerWins_));
    ui->labelSessionRound->setText("Round:  " +
                                   QString::number(sessionRound_));
}

void MainWindow::refreshHistoryTable() {
    QTableWidget* table = ui->tableHistory;
    table->setRowCount(matchHistory_.size());

    for (int i = 0; i < matchHistory_.size(); ++i) {
        const MatchRecord& r = matchHistory_[i];

        QString scoreStr = r.soloScore.isEmpty()
                         ? QString::number(r.playerWins) + "—" +
                           QString::number(r.computerWins)
                         : r.soloScore;

        auto* rdItem  = new QTableWidgetItem(QString::number(r.roundNumber));
        auto* winItem = new QTableWidgetItem(r.winner);
        auto* scItem  = new QTableWidgetItem(scoreStr);

        rdItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        winItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        scItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        QColor rowColor = r.soloScore.isEmpty()
                        ? (r.winner == playerName_ ? QColor(0,80,40) : QColor(80,20,20))
                        : QColor(20, 40, 80);  // blue for solo rounds

        rdItem->setBackground(rowColor);
        winItem->setBackground(rowColor);
        scItem->setBackground(rowColor);

        table->setItem(i, 0, rdItem);
        table->setItem(i, 1, winItem);
        table->setItem(i, 2, scItem);
    }
    if (!matchHistory_.isEmpty())
        table->scrollToBottom();
}

// ── Responsive layout ──────────────────────────────────────────────────────
void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    int w          = width();
    int h          = height();
    int margin     = 10;
    int panelWidth = 220;
    int gameWidth  = w - panelWidth - margin * 3;

    ui->labelTitle->setGeometry(margin, margin, gameWidth, 40);

    int towHeight = h * 0.45;
    ui->tugOfWarWidget->setGeometry(margin, 55, gameWidth, towHeight);

    // Solo widget takes the same space as the tug widget
    SoloProgressWidget* spw = findChild<SoloProgressWidget*>("soloProgressWidget");
    if (spw) spw->setGeometry(margin, 55, gameWidth, towHeight);

    int qY = 55 + towHeight + 10;
    ui->labelQuestion->setGeometry(margin, qY, gameWidth, 45);

    int inputW = 300, inputX = margin + (gameWidth - inputW) / 2;
    ui->inputAnswer->setGeometry(inputX, qY + 55, inputW, 40);

    int timerW = 150, timerX = margin + (gameWidth - timerW) / 2;
    ui->labelTimer->setGeometry(timerX, qY + 105, timerW, 35);

    int panelX = w - panelWidth - margin;
    ui->panelScore->setGeometry(panelX, margin, panelWidth, h - margin * 2);
    ui->labelPanelTitle->setGeometry(panelX + 5, margin + 10,  panelWidth - 10, 25);
    ui->labelSessionRound->setGeometry(panelX + 5, margin + 40,  panelWidth - 10, 22);
    ui->labelSessionPlayer->setGeometry(panelX + 5, margin + 65,  panelWidth - 10, 22);
    ui->labelSessionComputer->setGeometry(panelX + 5, margin + 90,  panelWidth - 10, 22);
    ui->labelHistoryTitle->setGeometry(panelX + 5, margin + 120, panelWidth - 10, 22);
    ui->tableHistory->setGeometry(panelX + 5, margin + 145,
                                   panelWidth - 10, h - margin * 2 - 155);
}
