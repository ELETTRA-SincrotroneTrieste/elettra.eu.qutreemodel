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
    /*!
     * \brief add a new item to the tree according to the string fields separated by a given separator
     *        (default: /)
     * \param s a string made up of an arbitrary number of fields separated by "/" or any other separator
     *        specified by setSeparator
     * \return if the item has been successfully added, the last item (leaf) is return. An invalid
     *         QModelIndex is returned otherwise.
     */
    virtual QModelIndex addItem(const QString& s) = 0;

    /*!
     * \brief find an item matching the given input string and remove it
     * \param s a string made up of sections separated by "/" or another separator specified with setSeparator
     * \return the item immediately above the removed one, if possible
     */
    virtual QModelIndex removeItem(const QString &s) = 0;

    /*!
     * \brief find an item matching the given input string
     * \param s a string made up of sections separated by "/" or another separator specified with setSeparator
     * \return the *leaf* QModelIndex corresponding to the *last section* of *s* if *s* has a match. An invalid
     *         QModelIndex otherwise
     */
    virtual QModelIndex findItem(const QString &s) = 0;

    /*!
     * \brief recursively find all items matching the given search string.
     * \param search the string to be searched across all children recursively
     * \param match_mode specify the match criterion. The default is Qt::MatchContains, and it's currently the
     *        only supported mode.
     * \param role match against data specified in the Qt item data role. Default: DisplayRole
     * \return a list of all QModelIndex matching the search pattern and the other criteria.
     */
    virtual QList<QModelIndex> findItems(const QString& search, int match_mode = Qt::MatchContains, int role = Qt::DisplayRole) const = 0;

    /*!
     * \brief return the list of indexes (below the given parent) that have children.
     *        This can be used to expand the whole tree or a section of the tree under a given parent.
     * \param parent the index under which the search starts
     * \return a list of QModelIndex who have at least one child
     */
    virtual QList<QModelIndex> itemsWithChildren(const QModelIndex& parent)  = 0;

    /*!
     * \brief prints data in idx display role as string, and recursively up until the root
     * \param idx the idex leaf
     * \return string representation of the item from the root to idx
     */
    virtual QString itemRepr(const QModelIndex& idx) = 0;

    /*!
     * \brief returns the QAbstractItemModel in use
     * \return QAbstractItemModel
     */
    virtual QAbstractItemModel *qmodel() = 0;

    /*!
     * \brief specify a custom separator instead of the default "/"
     * \param s the desired separator
     */
    virtual void setSeparator(const QString& s) = 0;

    /*!
     * \brief returns the separator used by addItem, findItem and all methods that operate on the
     *        tree by a string made up of sections to find and build items.
     * \return the separator in use.
     */
    virtual QString separator() const = 0;
};

/** \brief Interface for a plugin implementing a tree item model that can be used by custom
 *         tree views or QTreeView or QTreeWidget and so on.
 *
 * \paragraph Description
 *
 * After creating the plugin, use the instantiate method to create the tree model.
 * *instantiate* shall return a pointer to a QuTreeModelI, the interface to operate on the tree
 * model.
 *
 * \code
    CuPluginLoader loader;
    QObject **qo = nullptr;
    tree_i = loader.get<QuTreeModelPluginInterface>(QuTreeModelPluginInterface::file_name, qo);
    if(!tree_i)
        perr("%s: plugin \"%s\" not found", __FUNCTION__, QuTreeModelPluginInterface::file_name);
    else {
        QuTreeModelI *mi = tree_i->instantiate({"My Tree"}, this);
        const QStringList& args { "a/b/c/d", "a/b/d", "i/j/k", "i/h/k", "i/j/l/m" };
        foreach(QString s, args) {
            m_model_i->addItem(s);
        }
        // expand all tree
        foreach(const QModelIndex& i, m_model_i->itemsWithChildren(QModelIndex()))
            findChild<QTreeView *>()->expand(i);
    }
 * \endcode
 *
 * \paragraph Find and expand item(s)
 *
 * \code
    void MyTree::m_filter(const QString &s) {
        QList<QModelIndex > il = m_model_i->findItems(s);
        foreach(const QModelIndex& i, il) {
            // use itemRepr to print a representation of the item as string fields + separator
            printf("%d,%d, %s\n", i.row(), i.column(), qstoc(m_model_i->itemRepr(i)));
            // expand the found item and all its parent items
            findChild<QTreeView *>()->expand(i);
            QModelIndex p(i.parent());
            while(p.isValid()) {
                findChild<QTreeView *>()->expand(p);
                p = p.parent();
        }
    }
    findChild<QTreeView *>()->keyboardSearch(s);
}
 * \endcode
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
