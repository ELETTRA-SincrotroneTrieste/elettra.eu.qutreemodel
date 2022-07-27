#include "treeview_tgtest.h"
// #include "ui_treeview_tgtest.h"

// cumbia
#include <cumbiapool.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <quapps.h>
#include <cupluginloader.h>
// cumbia

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTreeView>
#include <cucontrolsutils.h>
#include <qupalette.h>
#include <qustring.h>
#include <qutreemodelplugininterface.h>
#include <quwatcher.h>

Treeview_tgtest::Treeview_tgtest(CumbiaPool *cumbia_pool, QWidget *parent, const QStringList& devs, const QStringList &atts) :
    QWidget(parent)
{
    // cumbia
    CuModuleLoader mloader(cumbia_pool, &m_ctrl_factory_pool, &m_log_impl);
    cu_pool = cumbia_pool;
//    ui = new Ui::Treeview_tgtest;
//    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

    CuPluginLoader loader;
    QObject **qo = nullptr;
    tree_i = loader.get<QuTreeModelPluginInterface>(QuTreeModelPluginInterface::file_name, qo);
    if(!tree_i)
        perr("%s: plugin \"%s\" not found", __FUNCTION__, QuTreeModelPluginInterface::file_name);
    else {


        //    ui = new Ui::Alarmmanager;
        //    ui->setupUi(this, cu_pool, m_ctrl_factory_pool);

        // mloader.modules() to get the list of loaded modules
        // cumbia

        m_attributes = atts;
        QuTreeModelI *mi = tree_i->instantiate(QStringList{ "device "} + m_attributes, this);
        m_model = mi->qmodel();
        m_model_i = mi;

        m_load(devs);

        QGridLayout *lo = new QGridLayout(this);
        lo->setObjectName("lo0");

        // tree view
        QTreeView *t = new QTreeView(this);
        lo->addWidget(t, 0, 0, 10, 4);
        t->setModel(this->m_model_i->qmodel());
        foreach(const QModelIndex &i , m_model_i->itemsWithChildren(QModelIndex()))
            t->expand(i);

        QLabel *search_l = new QLabel("Search", this);
        QLineEdit *search_le = new QLineEdit(this);
        search_le->setPlaceholderText("Type text to search...");
        connect(search_le, SIGNAL(textChanged(QString)), this, SLOT(m_filter(QString)));
        lo->addWidget(search_l, 10, 0, 1, 1);
        lo->addWidget(search_le, 10, 1, 1, 3);

        resize(1024, 768);
    }


    // mloader.modules() to get the list of loaded modules
    // cumbia
}

Treeview_tgtest::~Treeview_tgtest()
{
//    delete ui;
}

void Treeview_tgtest::m_filter(const QString &s) {
    QList<QModelIndex > il = m_model_i->findItems(s);
    foreach(const QModelIndex& i, il) {
        printf("%d,%d, %s\n", i.row(), i.column(), qstoc(m_model_i->printItem(i)));
        findChild<QTreeView *>()->expand(i);
        QModelIndex p(i.parent());
        while(p.isValid()) {
            findChild<QTreeView *>()->expand(p);
            p = p.parent();
        }
    }
    findChild<QTreeView *>()->keyboardSearch(s);
}

void Treeview_tgtest::m_load(const QStringList& args) {
    foreach(QString s, args) {
        s.remove(QRegularExpression("tango://"));
        m_model_i->addItem(s);
        for(int i = 1; i < m_model->columnCount(); i++) {
            QString a = m_model->headerData(i, Qt::Horizontal).toString();
            QuWatcher * w = new QuWatcher(this, cu_pool, m_ctrl_factory_pool);
            connect(w, SIGNAL(newData(CuData)), this, SLOT(m_update(CuData)));
            w->setSource(s + "/" + a);
        }
    }
}

void Treeview_tgtest::m_update(const CuData &da) {
    QuPalette palette;
    QuString s(da.s("src"));
    // first column is device name, followed by columns with m_attributes
    int c = m_attributes.indexOf(s.section('/', -1, -1)) + 1;
    QString dev = s.section('/', 0, s.count('/')-1);
    dev.remove(QRegularExpression("[A-Za-z0-9\\.\\-]+:\\d+/"));
    QModelIndex i = m_model_i->findItem(dev);
    printf("update data %s %s found idx %d,%d %s INDEX in header %d \n", qstoc(dev), qstoc(s.section('/', -1, -1)),
           i.row(), i.column(), i.data().toString().toStdString().c_str(),c);
    if(i.isValid()) {
        QModelIndex ai = m_model->index(i.row(), c, i.parent());
        if(ai.isValid()) {
            printf("\e[1;32mupdating at row %d col %d of '%s' column count %d\e[0m\n",
                   ai.row(), ai.column(), qstoc(ai.data().toString()), m_model->columnCount(ai));
            m_model->setData(ai, da["value"].toString().c_str(), Qt::DisplayRole);
            m_model->setData(ai, da.s("msg").length() > 0 ? da.s("msg").c_str() : "-", Qt::ToolTipRole);
            if(da.containsKey("qc"))
                m_model->setData(ai, palette[QString::fromStdString(da["qc"].toString())], Qt::BackgroundRole);
        }
        else
            perr("invalid sibling of %s index at column %d (attr %s)", qstoc(i.data().toString()), c, qstoc(s.section('/', -1, -1)));
    }

}
