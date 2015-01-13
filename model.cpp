#include "model.h"
#include <memory>

using namespace std;

Model::Model()
    : db(0)
{
}

void Model::dballe_connect(const std::string &dballe_url)
{
    using namespace dballe;

    if (db)
    {
        delete db;
        db = 0;
    }

    auto_ptr<DB> new_db = DB::connect_from_url(dballe_url.c_str());
    db = new_db.release();
}
