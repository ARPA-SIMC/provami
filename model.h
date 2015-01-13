#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <dballe/db/db.h>
#include <string>

class Model : public QObject
{
protected:
    dballe::DB* db;

public:
    Model();

    void dballe_connect(const std::string& dballe_url);
};

#endif // MODEL_H
