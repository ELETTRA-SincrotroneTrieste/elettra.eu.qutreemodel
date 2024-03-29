#ifndef QUTREEMODEL_H
#define QUTREEMODEL_H

#include <QAbstractItemModel>
#include <qutreemodelplugininterface.h>

class QAbstractItemView;

class QuTreeModel : public QAbstractItemModel, public QuTreeModelI
{
    Q_OBJECT

public:
    QuTreeModel(const QStringList &headers,
              QObject *parent = nullptr);
    ~QuTreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    bool insertColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;

    //
    // QuTreeModelI interface
    //
    void setSeparator(const QString& s) override;
    QString separator() const override;

    QModelIndex addItem(const QString& s) override;
    QModelIndex findItem(const QString &s) override;
    QModelIndex removeItem(const QString& s) override;
    virtual void expand(const QModelIndex& leaf, QAbstractItemView *view) const override;
    virtual QModelIndex select(const QString& match, QAbstractItemView *view) override;
    virtual QList<QModelIndex> findItems(const QString& search, int min_pattern_len, QAbstractItemView* view, int match_mode = Qt::MatchContains, int role = Qt::DisplayRole) override;
    virtual QList<QModelIndex> itemsWithChildren(const QModelIndex &parent) override;
    virtual QString itemRepr(const QModelIndex& idx) const override;
    virtual QStringList items() const override;


    virtual void highlight(const QModelIndex &mi) override;
    virtual void clearHighlight(const QModelIndex &mi)  override;

    virtual QAbstractItemModel *qmodel() override;

private:
    void setupModelData(const QStringList &lines, QuTreeItem *parent);
    QuTreeItem *getItem(const QModelIndex &index) const;

    QModelIndex m_add_item(const QString& s, const QModelIndex &parent = QModelIndex());
    QModelIndex m_find_item(const QString &s, int column = 0, const QModelIndex& parent = QModelIndex(), int role = Qt::DisplayRole);
    QList<QModelIndex> m_find_items(const QRegularExpression &re, int min_pattern_len, QAbstractItemView *view, int match_mode, const QModelIndex& parent, int role) ;
    QList<QModelIndex> m_leaf_items(const QModelIndex& parent) const;
    int m_get_idx(const QString& s, const QModelIndex& parent) const;
    QuTreeModelPrivate *d;
};


#endif // QUTREEMODEL_H
