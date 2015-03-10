/**
 * Copyright (C) 2007--2015  ARPA-SIM <urpsim@smr.arpa.emr.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Author: Enrico Zini <enrico@enricozini.com>
 */
#ifndef PROVAMI_TEST_UTILS_H
#define PROVAMI_TEST_UTILS_H

#include <wibble/tests.h>
#include <memory>

namespace provami {
namespace tests {

struct Check
{
    virtual ~Check() {}
    virtual void check(WIBBLE_TEST_LOCPRM) const = 0;
};

}
}

namespace wibble {
namespace tests {

static inline void _wassert(WIBBLE_TEST_LOCPRM, std::auto_ptr<provami::tests::Check> expr)
{
    expr->check(wibble_test_location);
}

}
}

#endif