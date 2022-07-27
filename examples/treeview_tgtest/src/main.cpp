#include <quapplication.h>
#include "treeview_tgtest.h"

// cumbia
#include <cumbiapool.h>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
// cumbia

#define VERSION "1.0"

int main(int argc, char *argv[])
{
    QuApplication qu_app( argc, argv );
    qu_app.setOrganizationName("Elettra");
    qu_app.setApplicationName("Treeview_tgtest");
    QString version(VERSION);
    qu_app.setApplicationVersion(version);
    qu_app.setProperty("author", "Giacomo");
    qu_app.setProperty("mail", "giacomo.strangolino@elettra.eu");
    qu_app.setProperty("phone", "0403758073");
    qu_app.setProperty("office", "T2PT025");
    qu_app.setProperty("hwReferent", "$HW_REFERENT$"); /* name of the referent that provides the device server */

    // $palette$

    QStringList devs, atts;
    for(int i = 1; i < argc; i++) {
        const QString s(argv[i]);
        if(s.count('/') >= 2) { // probably a device
            devs << s;
        }
        else if(s.count('/') == 0)
            atts << s;
    }
    if(devs.size() == 0 || atts.size() == 0) {
        perr("%s: usage: %s test/device/1 test/device/2 sys/tg_test/1 double_scalar long_scalar short_scalar",
             __PRETTY_FUNCTION__, argv[0]);
        exit(1);
    }

    CumbiaPool *cu_p = new CumbiaPool();
    Treeview_tgtest *w = new Treeview_tgtest(cu_p, nullptr, devs, atts);
    w->show();
    // exec application loop
    int ret = qu_app.exec();
    // delete resources and return
    delete w;

    for(std::string n : cu_p->names())
        if(cu_p->get(n))
            delete cu_p->get(n);

    return ret;
}
