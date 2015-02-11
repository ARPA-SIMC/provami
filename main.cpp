#include "provami/provamimainwindow.h"
#include "provami/model.h"
#include <dballe/core/record.h>
#include <QApplication>
#include <QDebug>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace dballe;
using namespace provami;

int main(int argc, char *argv[])
{
    Model model;
    QApplication a(argc, argv);
    ProvamiMainWindow w(model);

    // Parse initial query from command line arguments, taking those arguments
    // that contain an =
    Record initial_query;
    vector<string> args;
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-' || strchr(argv[i], '=') == NULL)
            args.push_back(argv[i]);
        else
            initial_query.set_from_string(argv[i]);
    }

    model.set_initial_filter(initial_query);

    if (args.empty())
    {
        const char* connect_url = getenv("DBA_DB");
        if (connect_url != NULL)
            model.dballe_connect(connect_url);
    } else {
        model.dballe_connect(args[0]);
    }
    w.show();
    qDebug() << "entering event loop" << endl;
    return a.exec();
}
