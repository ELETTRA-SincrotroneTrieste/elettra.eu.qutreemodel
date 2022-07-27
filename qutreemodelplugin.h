#ifndef QUTREEMODELPLUGIN_H
#define QUTREEMODELPLUGIN_H

#include <qutreemodelplugininterface.h>

#include <QObject>

class QuTreeModelPlugin : public QObject, public QuTreeModelPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "eu.elettra.qutreemodel" FILE "qutreemodelplugin.json")
    Q_INTERFACES(QuTreeModelPluginInterface)
public:
    explicit QuTreeModelPlugin(QObject *parent = nullptr);
    virtual ~QuTreeModelPlugin() {}

signals:

    // QuTreeModelPluginInterface interface
public:
    QuTreeModelI *instantiate(const QStringList &headers, QObject *parent);
};

#endif // QUTREEMODELPLUGIN_H
