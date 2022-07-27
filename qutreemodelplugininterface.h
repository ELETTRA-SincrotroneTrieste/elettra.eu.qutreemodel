#ifndef QUTREEMODELPLUGININTERFACE_H
#define QUTREEMODELPLUGININTERFACE_H

#include <QObject>
#include <cupluginloader.h>
#include <cumacros.h>

class QAbstractItemModel;

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class QuTreeItem;
class QuTreeModelPrivate;

class QuTreeModelI {
public:
    virtual bool addItem(const QString& s) = 0;
    virtual QModelIndex findItem(const QString &s) = 0;
    virtual QList<QModelIndex> findItems(const QString& search, int match_mode = Qt::MatchContains, int role = Qt::DisplayRole) const = 0;
    virtual QList<QModelIndex> itemsWithChildren(const QModelIndex& parent)  = 0;

    /*!
     * \brief prints data in idx display role as string, and recursively up until the root
     * \param idx the idex leaf
     * \return string representation of the item from the root to idx
     */
    virtual QString printItem(const QModelIndex& idx) = 0;

    virtual QAbstractItemModel *qmodel() = 0;

    virtual void setSeparator(const QString& s) = 0;
    virtual QString separator() const = 0;
};

/** \brief Interface for a plugin implementing reader that connects to multiple quantities.
 *
 * \ingroup plugins
 *
 *
 */
class QuTreeModelPluginInterface {
public:
    virtual ~QuTreeModelPluginInterface() { }
    virtual QuTreeModelI *instantiate(const QStringList &headers, QObject *parent) = 0;
    static constexpr const char file_name[32] = "libqutreemodel-plugin.so";
};


#define QuTreeModelPluginInterface_iid "eu.elettra.qutreemodel"
Q_DECLARE_INTERFACE(QuTreeModelPluginInterface, QuTreeModelPluginInterface_iid)

#endif // QUTREEMODELPLUGININTERFACE_H
