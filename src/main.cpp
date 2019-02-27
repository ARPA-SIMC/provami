#include "provami/provamimainwindow.h"
#include "provami/model.h"
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
    QApplication::setApplicationVersion(PACKAGE_VERSION);
    QCoreApplication::setOrganizationName("ARPA");
    QCoreApplication::setOrganizationDomain("emr.it");
    QCoreApplication::setApplicationName("Provami");

    QCommandLineParser parser;
    parser.setApplicationDescription(R"(Graphical interface to navigate a DB-All.e database.

Example dballe_url values:
 - sqlite:file.sqlite or sqlite://file.sqlite
 - postgresql://user@host/db
 - mysql://[host][:port]/[database][?propertyName1][=propertyValue1]…
See https://github.com/ARPA-SIMC/dballe/blob/master/doc/fapi_connect.md)");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("dballe_url", QCoreApplication::translate("main", "DB-All.e URL to the database to open."));
    parser.addPositionalArgument("key=val", QCoreApplication::translate("main", "initial filter constraints"), "[key=val...]");

    parser.process(app);

    // Parse initial query from command line arguments, taking those arguments
    // that contain an =
    core::Query initial_query;
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

        initial_query.set_from_string(arg.toUtf8().constData());
    }

    // Connect to the db
    if (non_query_args.empty())
    {
        const char* connect_url = getenv("DBA_DB");
        if (connect_url != NULL)
            model.dballe_connect(connect_url);
        else
            parser.showHelp(1);
    } else {
        model.dballe_connect(non_query_args[0]);
    }

    try {
        // Show the main window before running a refresh
        ProvamiMainWindow w(model);
        w.show();
        QApplication::processEvents();

        model.set_initial_filter(initial_query);

        return app.exec();
    } catch (std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }
}
