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
    QCoreApplication::setOrganizationName("ARPA");
    QCoreApplication::setOrganizationDomain("emr.it");
    QCoreApplication::setApplicationName("Provami");

    QCommandLineParser parser;
    parser.setApplicationDescription("Graphical interface to navigate a DB-All.e database");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("dballe_url", QCoreApplication::translate("main", "DB-All.e URL to the database to open."));
    parser.addPositionalArgument("key=val", QCoreApplication::translate("main", "initial filter constraints"), "[key=val...]");

    parser.process(app);

    ProvamiMainWindow w(model);

    // Parse initial query from command line arguments, taking those arguments
    // that contain an =
    auto initial_query = Record::create();
    auto args = parser.positionalArguments();
    vector<string> non_query_args;
    for (const auto& arg: args)
    {
        // Split the input as name=val
        int split = arg.indexOf('=');
        if (split == -1)
        {
            non_query_args.push_back(arg.toStdString());
            continue;
        }

        initial_query->setf(
                (const char*)arg.left(split).toUtf8().constData(),
                (const char*)arg.right(arg.size() - split - 1).toUtf8().constData());
    }

    auto initial_filter = Query::create();
    initial_filter->set_from_record(*initial_query);
    model.set_initial_filter(*initial_filter);

    // Connect to the db
    if (non_query_args.empty())
    {
        const char* connect_url = getenv("DBA_DB");
        if (connect_url != NULL)
            model.dballe_connect(connect_url);
    } else {
        model.dballe_connect(non_query_args[0]);
    }

    // Show the main window before running a refresh
    w.show();
    QApplication::processEvents();

    model.activate_next_filter();

    return app.exec();
}
