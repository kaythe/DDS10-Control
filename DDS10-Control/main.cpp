#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "dds10.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<DDS10>("dds10", 0, 1, "DDS10");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    DDS10 dds;
    engine.rootContext()->setContextProperty("dds", &dds);

    return app.exec();
}
