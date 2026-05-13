# MathTagOfWar — Qt Edition 🧮⚔️

A fully graphical math competition game built in **C++ with Qt6**, where players compete in a tug-of-war by answering math questions. Answer correctly and pull your opponent across the line — answer wrong and get dragged back.

> **Status:** 🚧 Active development — core game is fully playable. Online multiplayer and enhanced character graphics planned.

---

## Screenshots

> *Game running at Primary Easy difficulty — Player vs Computer*
><img width="1312" height="805" alt="image" src="https://github.com/user-attachments/assets/c1488a66-5adf-410f-acf3-fa4b17f5146e" />



---

## Features

| Feature | Status |
|---|---|
| Graphical tug-of-war with animated rope and silhouette figures | ✅ |
| Smooth rope animation using Qt property animation | ✅ |
| Player vs Computer mode | ✅ |
| 4 difficulty levels based on British curriculum (KS1–GCSE) | ✅ |
| Player name entry at game start | ✅ |
| Countdown timer per question (10 seconds) | ✅ |
| Session score panel with cumulative match history | ✅ |
| Responsive layout — scales with window size | ✅ |
| Play again without losing session scores | ✅ |
| New session reset | ✅ |
| Online multiplayer | 🔜 Planned |
| Realistic character graphics | 🔜 Planned |
| Sound effects | 🔜 Planned |

---

## Difficulty Levels

Based on the **British National Curriculum**:

| Level | Curriculum Stage | Age | Topics |
|---|---|---|---|
| **Primary Easy** | KS1 / KS2 Early | 5–8 | Addition, subtraction, simple times tables (up to 5×5) |
| **Primary Hard** | KS2 Late | 9–11 | Full 12×12 times tables, division, BODMAS, squares & square roots |
| **Secondary Easy** | KS3 | 11–14 | Linear equations, percentages, negative numbers, powers, fractions |
| **Secondary Hard** | KS4 (GCSE) | 14–16 | Quadratics, simultaneous equations, Pythagoras, index laws, ratio |

---

## Gameplay

1. Enter your name and select a difficulty level
2. A math question appears on screen with a **10 second countdown**
3. Type your answer and press **Enter**
4. The computer also attempts the question simultaneously
5. Correct answer → rope pulls toward you
6. Wrong answer → computer pulls the rope
7. Time runs out → computer gets a free pull
8. First to pull the rope fully to their side wins the round
9. Session scores accumulate across multiple rounds — track your wins in the side panel

---

## C++ and Qt Concepts Demonstrated

- **OOP architecture** — separate classes for `Player`, `ComputerPlayer`, `Question`, `QuestionGenerator`, `ScoreTracker`, `TugOfWarWidget`
- **Inheritance & polymorphism** — `ComputerPlayer` inherits from `Player`, overrides `isComputer()` and answer simulation
- **Qt Widgets** — `QMainWindow`, `QDialog`, `QLabel`, `QLineEdit`, `QTableWidget`, `QPushButton`
- **Custom painting** — `TugOfWarWidget` overrides `paintEvent()` using `QPainter`, `QPainterPath`, `QLinearGradient`, `QRadialGradient`
- **Qt Animation** — `QPropertyAnimation` with `OutCubic` easing for smooth rope movement
- **Signals & slots** — timer ticks, answer submission, button clicks all connected via Qt's signal/slot system
- **Dynamic layout** — `resizeEvent()` override for responsive widget positioning

---

## Project Structure

```
MathTugOfWar-Qt/
│
├── main.cpp                  ← App entry point
├── mainwindow.h/.cpp         ← Main game controller — UI, game loop, scoring
├── mainwindow.ui             ← Qt Designer UI file
├── tugofwarwidget.h/.cpp     ← Custom painted widget — rope, figures, animation
│
├── Player.h/.cpp             ← Base player class
├── ComputerPlayer.h/.cpp     ← AI opponent (inherits Player)
├── Question.h/.cpp           ← Question data and answer checking
├── QuestionGenerator.h/.cpp  ← Generates questions by difficulty level
├── ScoreTracker.h/.cpp       ← Tracks round wins across sessions
└── Difficulty.h              ← Difficulty enum (PRIMARY_EASY → SECONDARY_HARD)
```

---

## Requirements

- **Qt 6.x** (tested on Qt 6.11)
- **C++17** compiler (MinGW 64-bit on Windows, GCC on Linux/Mac)
- **Qt Widgets** and **Qt Core** modules

---

## How to Build and Run

### Option 1 — Qt Creator (recommended)

1. Open Qt Creator
2. File → Open Project → select `MathTugOfWar_Qt.pro`
3. Select your Qt kit (MinGW 64-bit)
4. Press **Ctrl+B** to build
5. Press **Ctrl+R** to run

### Option 2 — Command line

```bash
qmake MathTugOfWar_Qt.pro
make
./MathTugOfWar_Qt
```

---

## Roadmap

- [x] Core game loop and question/answer mechanic
- [x] Computer opponent with difficulty-scaled accuracy and response time
- [x] Graphical tug-of-war display with animated rope
- [x] Silhouette player figures with glow effects
- [x] Smooth rope animation (QPropertyAnimation)
- [x] Difficulty selection screen with player name entry
- [x] Countdown timer with colour change warnings
- [x] Session score panel and match history table
- [x] Responsive window layout
- [x] Play again and new session options
- [ ] Realistic character graphics to replace silhouettes
- [ ] Both hands gripping rope
- [ ] Sound effects and background music
- [ ] Online multiplayer (each player on their own device)
- [ ] Secondary Hard difficulty expanded question bank
- [ ] High score leaderboard saved to file

---

## Related Repository

The original terminal version of this game (pure C++, no Qt) is available here:
👉 [MathTagOfWar — Terminal Version](https://github.com/Phelnestq/MathTugOfWar)

The Qt version shares the core game logic (`Player`, `Question`, `QuestionGenerator`, `ComputerPlayer`, `ScoreTracker`) and replaces the terminal display with a full graphical interface.

---

## Why I Built This

I wanted a meaningful project that applied OOP principles to something real and interactive. Designing a game for children forced me to think about user experience, difficulty scaling, clean class architecture, and eventually graphical rendering — skills that go well beyond textbook exercises.

The project also served as my introduction to Qt — learning custom widget painting, property animation, signal/slot architecture, and responsive layouts through a hands-on build rather than tutorials alone.

---

## Author

M.Sc. Computer Simulation student at Bergische Universität Wuppertal
Interests: scientific computing, data analysis, simulation tooling, and building useful software
