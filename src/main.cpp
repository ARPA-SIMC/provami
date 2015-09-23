#include "provami/provamimainwindow.h"
#include "provami/model.h"
#include <dballe/record.h>
#include <dballe/query.h>
#include <QMetaType>
#include <QApplication>
#include <QNetworkProxyFactory>
#include <QCommandLineParser>
#include <QDebug>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace wreport;
using namespace dballe;
using namespace provami;

int main(int argc, char *argv[])
{
    qRegisterMetaType<dballe::core::Query>("dballe::core::Query");

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    Model model;
    QApplication app(argc, argv);
    QApplication::setApplicationName("provami");
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Graphical interface to navigate a DB-All.e database");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    ProvamiMainWindow w(model);

    // Parse initial query from command line arguments, taking those arguments
    // that contain an =
    auto initial_query = Record::create();
    vector<string> args;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-' || strchr(argv[i], '=') == NULL)
            args.push_back(argv[i]);
        else
        {
            // Split the input as name=val
            const char* str = argv[i];
            const char* s = strchr(str, '=');
            if (!s) error_consistency::throwf("there should be an = between the name and the value in '%s'", str);
            string key(str, s - str);
            initial_query->setf(key.c_str(), s + 1);
        }
    }

    auto initial_filter = Query::create();
    initial_filter->set_from_record(*initial_query);
    model.set_initial_filter(*initial_filter);

    if (args.empty())
    {
        const char* connect_url = getenv("DBA_DB");
        if (connect_url != NULL)
            model.dballe_connect(connect_url);
    } else {
        model.dballe_connect(args[0]);
    }
    model.refresh_thread.start();
    w.show();
    qDebug() << "entering event loop" << endl;
    return app.exec();
}
