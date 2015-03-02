#ifndef PROVAMI_CONFIG_H
#define PROVAMI_CONFIG_H

#include <QDir>

namespace provami {

class Config
{
public:
    QDir data_dir;

    Config();

    static const Config& get();
};

}

#endif
