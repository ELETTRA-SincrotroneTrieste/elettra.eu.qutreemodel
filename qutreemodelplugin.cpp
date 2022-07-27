#include "qutreemodelplugin.h"
#include "qutreemodel.h"

QuTreeModelPlugin::QuTreeModelPlugin(QObject *parent)
    : QObject{parent} {

}

QuTreeModelI *QuTreeModelPlugin::instantiate(const QStringList &headers, QObject *parent) {
    QuTreeModelI *i = new QuTreeModel(headers, parent);
    return i;
}
