#include "provami/config.h"
#include <QDebug>

namespace provami {

Config::Config()
{
    data_dir.setPath(".");
    if (!data_dir.exists("mapview"))
        data_dir.setPath(DATADIR);
}

const Config &Config::get()
{
    static Config* cfg = 0;
    if (!cfg) cfg = new Config;
    return *cfg;
}



}
