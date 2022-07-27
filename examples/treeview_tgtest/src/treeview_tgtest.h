#ifndef Treeview_tgtest_H
#define Treeview_tgtest_H

#include <QWidget>

// cumbia
#include <qulogimpl.h>
#include <cucontrolsfactorypool.h>
class CumbiaPool;
// cumbia

class QuTreeModelPluginInterface;
class QAbstractItemModel;
class QuTreeModelI;
class CuData;

namespace Ui {
class Treeview_tgtest;
}

class Treeview_tgtest : public QWidget
{
    Q_OBJECT

public:
    explicit Treeview_tgtest(CumbiaPool *cu_p, QWidget *parent, const QStringList &devs, const QStringList &atts);
    ~Treeview_tgtest();

private:
    Ui::Treeview_tgtest *ui;

    // cumbia
    CumbiaPool *cu_pool;
    QuLogImpl m_log_impl;
    CuControlsFactoryPool m_ctrl_factory_pool;
    // cumbia

    QuTreeModelPluginInterface *tree_i;
    QAbstractItemModel *m_model;
    QuTreeModelI *m_model_i;

    QStringList m_attributes;

private slots:
    void m_filter(const QString& s);
    void m_load(const QStringList &args);
    void m_update(const CuData& da);
};

#endif // Treeview_tgtest_H
