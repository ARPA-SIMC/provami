#include "provami/rawquerymodel.h"
#include <QDebug>

using namespace dballe;
using namespace wreport;
using namespace std;

namespace provami {

RawQueryModel::RawQueryModel(Model& model, QObject *parent) :
    QAbstractTableModel(parent), model(model)
{
    QObject::connect(&model, SIGNAL(next_filter_changed()), this, SLOT(next_filter_changed()));
}

RawQueryModel::ColumnType RawQueryModel::resolveColumnType(int column) const
{
    switch (column)
    {
    case 0: return CT_KEY;
    case 1: return CT_VALUE;
    default: return CT_INVALID;
    }
}

int RawQueryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return values.size();
}

int RawQueryModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return 2;
}

QVariant RawQueryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if ((unsigned)index.row() >= values.size()) return QVariant();
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        const rawquery::Item& item = values[index.row()];
        switch (ctype)
        {
        case CT_KEY: return QVariant(item.key.c_str());
        case CT_VALUE: return QVariant(item.val.c_str());
        default: return QVariant();
        }
        break;
    }
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
        return QVariant();
    }
    return QVariant();
}

QVariant RawQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation != Qt::Horizontal)
        return QVariant();

    switch (resolveColumnType(section))
    {
    case CT_KEY: return QVariant("Key");
    case CT_VALUE: return QVariant("Value");
    default: return QVariant();
    }
}

Qt::ItemFlags RawQueryModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    if ((unsigned)index.row() >= values.size()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool RawQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) return false;
    if ((unsigned)index.row() >= values.size()) return false;
    ColumnType ctype = resolveColumnType(index.column());
    if (ctype == CT_INVALID) return false;

    // Update values
    switch (ctype)
    {
    case CT_KEY: values[index.row()].key = value.toString().toStdString(); break;
    case CT_VALUE: values[index.row()].val = value.toString().toStdString(); break;
    default: break;
    }

    auto query = build_query();
    model.set_filter(*query);

    return true;
}

const rawquery::Item* RawQueryModel::valueAt(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    if ((unsigned)index.row() >= values.size()) return NULL;
    const rawquery::Item& item = values[index.row()];
    return &item;
}

namespace {

struct VarGen
{
    std::vector<rawquery::Item>& items;

    VarGen(std::vector<rawquery::Item>& items) : items(items) {}

    void gen(const char* key, int val)
    {
        items.emplace_back(key, std::to_string(val));
    };

    void gen(const char* key, const char* val)
    {
        items.emplace_back(key, val);
    };

    void gen(const char* key, const std::string& val)
    {
        items.emplace_back(key, val);
    };

    void gen(const char* key, const Ident& val)
    {
        items.emplace_back(key, val.get());
    };

    void gen_lat(const char* key, int val)
    {
        char buf[15];
        snprintf(buf, 14, "%.5f", Coords::lat_from_int(val));
        gen(key, buf);
    }

    void gen_lon(const char* key, int val)
    {
        char buf[15];
        snprintf(buf, 14, "%.5f", Coords::lon_from_int(val));
        gen(key, buf);
    }
};

}

static std::vector<rawquery::Item> record_to_items(const dballe::Query& query)
{
    const auto& q = core::Query::downcast(query);
    std::vector<rawquery::Item> res;
    VarGen vargen(res);

    if (q.ana_id != MISSING_INT) vargen.gen("ana_id", q.ana_id);
    if (q.priomin != q.priomax)
    {
        if (q.priomin != MISSING_INT)
            vargen.gen("priority", q.priomin);
    } else {
        if (q.priomin != MISSING_INT)
            vargen.gen("priomin", q.priomin);
        if (q.priomax != MISSING_INT)
            vargen.gen("priomax", q.priomax);
    }
    if (!q.report.empty())
        vargen.gen("rep_memo", q.report);
    if (q.mobile != MISSING_INT)
        vargen.gen("mobile", q.mobile);
    if (!q.ident.is_missing())
        vargen.gen("ident", q.ident);
    if (!q.latrange.is_missing())
    {
        if (q.latrange.imin == q.latrange.imax)
            vargen.gen_lat("lat", q.latrange.imin);
        else
        {
            if (q.latrange.imin != LatRange::IMIN) vargen.gen_lat("latmin", q.latrange.imin);
            if (q.latrange.imax != LatRange::IMAX) vargen.gen_lat("latmax", q.latrange.imax);
        }
    }
    if (!q.lonrange.is_missing())
    {
        if (q.lonrange.imin == q.lonrange.imax)
            vargen.gen_lon("lon", q.lonrange.imin);
        else
        {
            vargen.gen_lon("lonmin", q.lonrange.imin);
            vargen.gen_lon("lonmax", q.lonrange.imax);
        }
    }
    if (q.dtrange.min == q.dtrange.max)
    {
        if (!q.dtrange.min.is_missing())
        {
            vargen.gen("year",  q.dtrange.min.year);
            vargen.gen("month", q.dtrange.min.month);
            vargen.gen("day",   q.dtrange.min.day);
            vargen.gen("hour",  q.dtrange.min.hour);
            vargen.gen("min",   q.dtrange.min.minute);
            vargen.gen("sec",   q.dtrange.min.second);
        }
    } else {
        if (!q.dtrange.min.is_missing())
        {
            vargen.gen("yearmin",  q.dtrange.min.year);
            vargen.gen("monthmin", q.dtrange.min.month);
            vargen.gen("daymin",   q.dtrange.min.day);
            vargen.gen("hourmin",  q.dtrange.min.hour);
            vargen.gen("minumin",  q.dtrange.min.minute);
            vargen.gen("secmin",   q.dtrange.min.second);
        }
        if (!q.dtrange.max.is_missing())
        {
            vargen.gen("yearmax",  q.dtrange.max.year);
            vargen.gen("monthmax", q.dtrange.max.month);
            vargen.gen("daymax",   q.dtrange.max.day);
            vargen.gen("hourmax",  q.dtrange.max.hour);
            vargen.gen("minumax",  q.dtrange.max.minute);
            vargen.gen("secmax",   q.dtrange.max.second);
        }
    }
    if (q.level.ltype1 != MISSING_INT) vargen.gen("leveltype1", q.level.ltype1);
    if (q.level.l1     != MISSING_INT) vargen.gen("l1",         q.level.l1);
    if (q.level.ltype2 != MISSING_INT) vargen.gen("leveltype2", q.level.ltype2);
    if (q.level.l2     != MISSING_INT) vargen.gen("l2",         q.level.l2);
    if (q.trange.pind  != MISSING_INT) vargen.gen("pindicator", q.trange.pind);
    if (q.trange.p1    != MISSING_INT) vargen.gen("p1",         q.trange.p1);
    if (q.trange.p2    != MISSING_INT) vargen.gen("p2",         q.trange.p2);
    switch (q.varcodes.size())
    {
         case 0:
             break;
         case 1:
             vargen.gen("var", varcode_format(*q.varcodes.begin()));
             break;
         default: {
             string codes;
             for (const auto& code: q.varcodes)
             {
                 if (codes.empty())
                     codes = varcode_format(code);
                 else
                 {
                     codes += ",";
                     codes += varcode_format(code);
                 }
             }
             vargen.gen("varlist", codes);
             break;
         }
    }
    if (!q.query.empty()) vargen.gen("query", q.query);
    if (!q.ana_filter.empty())  vargen.gen("ana_filter",  q.ana_filter);
    if (!q.data_filter.empty()) vargen.gen("data_filter", q.data_filter);
    if (!q.attr_filter.empty()) vargen.gen("attr_filter", q.attr_filter);
    if (q.limit != MISSING_INT) vargen.gen("limit",       q.limit);
    if (q.block != MISSING_INT)   vargen.gen("block",   q.block);
    if (q.station != MISSING_INT) vargen.gen("station", q.station);

    return res;
}

void RawQueryModel::next_filter_changed()
{
    // Rebuild filter
    std::vector<rawquery::Item> new_items = record_to_items(model.explorer.get_filter());

    // Preserve the last row if partially edited, or add a new one
    if (!values.empty() && (values.back().key.empty() || values.back().val.empty()))
        new_items.emplace_back(values.back());
    else
        new_items.emplace_back();

    beginResetModel();
    values = new_items;
    endResetModel();
}

unique_ptr<Query> RawQueryModel::build_query() const
{
    core::Query* new_rec;
    std::unique_ptr<core::Query> res(new_rec = new core::Query);
    for (const auto& item: values)
    {
        if (item.key.empty() || item.val.empty()) continue;
        try {
            new_rec->set_from_string((item.key + "=" + item.val).c_str());
        } catch (std::exception&) {
            continue;
        }
    }
    return std::move(res);
}

static bool is_shell_safe(char c)
{
    if (isalnum(c)) return true;
    switch (c)
    {
    case '@':
    case '%':
    case '_':
    case '-':
    case '+':
    case '=':
    case ':':
    case ',':
    case '.':
    case '/':
        return true;
    }
    return false;
}

static std::string shell_escape(const std::string& s)
{
    if (s.empty()) return s;
    bool is_safe = true;
    for (auto c: s)
        is_safe = is_safe && is_shell_safe(c);
    if (is_safe) return s;
    std::string res("'");
    for (auto c: s)
        if (c == '\'')
            res += "'\\''";
        else
            res += c;
    res += '\'';
    return res;
}

QStringList RawQueryModel::as_shell_args(bool quoted) const
{
    QStringList res;
    std::vector<rawquery::Item> new_items = record_to_items(*build_query());
    for (auto item: new_items)
    {
        std::string s(item.key.c_str());
        s += "=";
        if (quoted)
            s += shell_escape(item.val);
        else
            s += item.val;
        res.append(s.c_str());
    }
    return res;
}


}
