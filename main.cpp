#include "qaction.h"
#include "qactiongroup.h"
#include "qlogging.h"
#include "qmenu.h"
#include "qstringview.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>
#include <QDebug>
#include <QFile>
#include <QActionGroup>
extern "C" {
    #include "fanctrl.h"
}

void iterate_string_add_QA(char str[], const char delim[], QActionGroup *strategyGroup, QMenu &menu) {
    char *line = strtok(str, "\n");
    while (line != NULL) {
        QAction *action = menu.addAction(line);
        action->setCheckable(true);
        strategyGroup->addAction(action);
        line = strtok(NULL, "\n");
    }
}

int main(int argc, char *argv[])
{
    qDebug() << "Qt library paths:";
    for (const QString &path : QCoreApplication::libraryPaths())
        qDebug() << path;

    qDebug() << "QT_PLUGIN_PATH:" << qEnvironmentVariable("QT_PLUGIN_PATH");
    qDebug() << "QT_QPA_PLATFORM:" << qEnvironmentVariable("QT_QPA_PLATFORM");
    qDebug() << "QT_PLUGIN_PATH env:" << qEnvironmentVariable("QT_PLUGIN_PATH");
    
    QApplication app(argc, argv);

    if (QSystemTrayIcon::isSystemTrayAvailable() == false) {
        qCritical() << "System tray is not available";
        return EXIT_FAILURE;
    }
    QSystemTrayIcon tray;
    tray.setToolTip("fw-fanctrl");
    tray.setIcon(QIcon(":/icons/framework_dark.png"));

    QMenu menu;
    tray.setContextMenu(&menu);

    QActionGroup *strategyGroup = new QActionGroup(&menu);
    strategyGroup->setExclusive(true);
    
    char active_strategy[108];
    char parsed_active_strategy[108];
    get_active_strat(active_strategy, sizeof(active_strategy));
    parse_active_strat(active_strategy, parsed_active_strategy);

    char all_strategies[108];
    char parsed_strategies[108];
    get_all_strats(all_strategies, sizeof(all_strategies));
    parse_stratlist(all_strategies, parsed_strategies);

    iterate_string_add_QA(parsed_strategies, "\n", strategyGroup, menu);

    for (QAction *action : strategyGroup->actions()) {
        if (action->text() == parsed_active_strategy) {
            action->setChecked(true);
            break;
        }
    }

    QObject::connect(strategyGroup, &QActionGroup::triggered, [&](QAction *action) {
        const QByteArray strategy = action->text().toUtf8();

        char response[108];

        int result = set_strat(strategy.constData(), response, sizeof(response));

        if (result < 0) {
            qWarning() << "Failed to set strategy:" << action->text();
        }
    });

    tray.show();
    int result = app.exec();
    return result;
}
