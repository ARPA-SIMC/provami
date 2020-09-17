PROVAMI
===============================================================

[![Build Status](https://badges.herokuapp.com/travis/ARPA-SIMC/provami?branch=master&env=DOCKER_IMAGE=centos:7&label=centos7)](https://travis-ci.org/ARPA-SIMC/provami)
[![Build Status](https://badges.herokuapp.com/travis/ARPA-SIMC/provami?branch=master&env=DOCKER_IMAGE=centos:8&label=centos8)](https://travis-ci.org/ARPA-SIMC/provami)
[![Build Status](https://badges.herokuapp.com/travis/ARPA-SIMC/provami?branch=master&env=DOCKER_IMAGE=fedora:31&label=fedora31)](https://travis-ci.org/ARPA-SIMC/provami)
[![Build Status](https://badges.herokuapp.com/travis/ARPA-SIMC/provami?branch=master&env=DOCKER_IMAGE=fedora:32&label=fedora32)](https://travis-ci.org/ARPA-SIMC/provami)
[![Build Status](https://badges.herokuapp.com/travis/ARPA-SIMC/provami?branch=master&env=DOCKER_IMAGE=fedora:rawhide&label=fedorarawhide)](https://travis-ci.org/ARPA-SIMC/provami)

[![Build Status](https://copr.fedorainfracloud.org/coprs/simc/stable/package/provami/status_image/last_build.png)](https://copr.fedorainfracloud.org/coprs/simc/stable/package/provami/)


Introduction
------------

**PLEASE NOTE: "provami" is deprecated. The new GUI application to visualise and navigate DB-All.e databases is: https://github.com/ARPA-SIMC/dballe-web**

"provami" is a GUI application to visualise and navigate DB-All.e databases.
(see https://github.com/ARPA-SIMC/dballe)

It also allows to perform simple editing tasks, and to graphically select and
export data subsets.

It's a qt version of the old "provami" included in DB-All.e sources.

Usage
-----

Usage: `provami-qt [options] dballe_url [key=val...]`

Example `dballe_url` values:
 - `sqlite:file.sqlite` or `sqlite://file.sqlite`
 - `postgresql://user@host/db`
 - `mysql://[host][:port]/[database][?propertyName1][=propertyValue1]`…
 
See https://arpa-simc.github.io/dballe/parms.html

Options:
  `-h`, `--help   `  Displays this help.
  `-v`, `--version`  Displays version information.

Arguments:
  `dballe_url`     DB-All.e URL to the database to open.
  `key=val   `     initial filter constraints


*Note*: a internet connection is necessary to load underlying maps. If the connection is managed by a proxy server it's necessary to export the `http_proxy` environment variable before launching the application, e.g.:
```
$ export http_proxy=http://USERNAME:PASSWORD@proxy-server.something:3128/
```


Contact and copyright information
---------------------------------

The author of DB-ALLe is Enrico Zini <enrico@enricozini.com>

Provami is Copyright (C) 2015-2020 ARPAE-SIMC <urpsim@arpae.it>

Provami is licensed under the terms of the GNU General Public License version 2.0
