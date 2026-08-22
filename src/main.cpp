#include "qlogging.h"
#include <QMenu>
#include <QIcon>
#include <QDebug>
#include <QMenu>
#include <QFile>
#include <cstdio>
#include <cstring>
#include <QAction>
#include <QObject>
#include <QtLogging>
#include <QStyleHints>
#include <QStringView>
#include <QApplication>
#include <QActionGroup>
#include <QApplication>
#include <QActionGroup>
#include <QSystemTrayIcon>

extern "C" {
    #include "include/fanctrl.h"
}

void iterate_string_add_QAction(char str[], const char delim[], QActionGroup *strategyGroup, QMenu &menu) {
    char *line = strtok(str, "\n");
    qInfo() << "Available Strategies:";
    while (line != NULL) {
        qInfo() << line;
        QAction *action = menu.addAction(line);
        action->setCheckable(true);
        strategyGroup->addAction(action);
        line = strtok(NULL, "\n");
    }
}

bool isDarkMode()
{
    /* Return true if Darkmode, false if Lightmode */
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
}

int main(int argc, char *argv[])
{
    qDebug() << "Qt library paths:";
    for (const QString &path : QCoreApplication::libraryPaths())
        qDebug() << path;

    QApplication app(argc, argv);

    if (QSystemTrayIcon::isSystemTrayAvailable() == false) {
        qCritical() << "System tray is not available";
        return EXIT_FAILURE;
    }

    QSystemTrayIcon tray;
    tray.setToolTip("fw-fanctrl");

    if (isDarkMode()) {
        tray.setIcon(QIcon(":/icons/framework_dark.png"));
    } else if (!isDarkMode()) {
        tray.setIcon(QIcon(":/icons/framework_light.png"));
    } else {
         // fallback to light and print warning
        tray.setIcon(QIcon(":/icons/framework_light.png"));
        qWarning() << "Failed to detect colorscheme. Falling back to light...";
    }

    QMenu menu;
    tray.setContextMenu(&menu);

    QActionGroup *strategyGroup = new QActionGroup(&menu);
    strategyGroup->setExclusive(true);

    QAction *quitButton = new QAction(&menu);
    quitButton->setText("Quit");
    quitButton->setEnabled(true);
    
    char active_strategy[256];
    char parsed_active_strategy[256] = "";
    int ret = get_active_strat(active_strategy, sizeof(active_strategy));
    if (ret < 0) {
        qWarning() << "Failed to get currently active strategy; aborting parsing";
    } else {
        ret = parse_active_strat(active_strategy, parsed_active_strategy, sizeof(parsed_active_strategy));
        if (ret < 0) {
            qWarning() << "Failed to parse active strategy";
        }
    }

    char all_strategies[256];
    char parsed_strategies[256] = "";
    ret = get_all_strats(all_strategies, sizeof(all_strategies));
    if (ret < 0) {
        qWarning() << "Failed to get all strats from socket; aborting parsing";
    } else {
        ret = parse_stratlist(all_strategies, parsed_strategies, sizeof(parsed_strategies));
        if (ret < 0) {
            qWarning() << "Failed to parse all strategies. There may be missing ones in the tray.";
        }
    }

    iterate_string_add_QAction(parsed_strategies, "\n", strategyGroup, menu);

    for (QAction *action : strategyGroup->actions()) {
        if (action->text() == parsed_active_strategy) {
            action->setChecked(true);
            break;
        }
    }

    menu.addSeparator();
    menu.addAction(quitButton);

    QObject::connect(strategyGroup, &QActionGroup::triggered, [&](QAction *action) {
        const QByteArray strategy = action->text().toUtf8();

        char response[108];

        int result = set_strat(strategy.constData(), response, sizeof(response));
        qInfo() << "Switching to" << action->text();
        if (result < 0) {
            qWarning() << "Failed to set strategy:" << action->text();
        }
    });

    QObject::connect(quitButton, &QAction::triggered, [=]() {
        qInfo() << quitButton->text() << "was clicked. Exiting...";
        QApplication::quit();
    });

    tray.show();
    int returncode = app.exec();
    return returncode;
}
