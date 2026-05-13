QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ComputerPlayer.cpp \
    Player.cpp \
    Question.cpp \
    QuestionGenerator.cpp \
    ScoreTracker.cpp \
    main.cpp \
    mainwindow.cpp \
    soloprogresswidget.cpp \
    tugofwarwidget.cpp

HEADERS += \
    ComputerPlayer.h \
    Difficulty.h \
    Player.h \
    Question.h \
    QuestionGenerator.h \
    ScoreTracker.h \
    mainwindow.h \
    soloprogresswidget.h \
    tugofwarwidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
