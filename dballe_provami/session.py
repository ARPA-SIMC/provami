import dballe
import asyncio
from collections import defaultdict
import concurrent.futures
import functools
import logging

log = logging.getLogger(__name__)


class Filter:
    def __init__(self):
        self.ana_id = None
        self.rep_memo = None
        self.level = None
        self.trange = None
        self.var = None
        self.datemin = None
        self.datemax = None

    def to_record(self):
        res = dballe.Record()
        if self.ana_id is not None: res["ana_id"] = self.ana_id
        if self.rep_memo is not None: res["rep_memo"] = self.rep_memo
        if self.level is not None: res["level"] = tuple(self.level)
        if self.trange is not None: res["trange"] = tuple(self.trange)
        if self.var is not None: res["var"] = self.var
        if self.datemin is not None: res["datemin"] = self.datemin
        if self.datemax is not None: res["datemax"] = self.datemax
        return res

    def to_dict(self):
        return {
            "ana_id": self.ana_id,
            "rep_memo": self.rep_memo,
            "level": self.level,
            "trange": self.trange,
            "var": self.var,
            "datemin": self.datemin.strftime("%Y-%m-%d %H:%M:%S") if self.datemin is not None else None,
            "datemax": self.datemax.strftime("%Y-%m-%d %H:%M:%S") if self.datemin is not None else None,
        }

    @classmethod
    def from_dict(cls, data):
        res = cls()
        res.ana_id = data.get("ana_id")
        res.rep_memo = data.get("rep_memo")
        res.level = data.get("level")
        res.trange = data.get("trange")
        res.var = data.get("var")
        res.datemin = data.get("datemin")
        res.datemax = data.get("datemax")
        return res


class Summary:
    def __init__(self, records):
        self.records = records
        self.ana_id = set()
        self.rep_memo = set()
        self.level = set()
        self.trange = set()
        self.var = set()
        self.datemin = None
        self.datemax = None
        for rec in records:
            self.ana_id.add(rec["ana_id"])
            self.rep_memo.add(rec["rep_memo"])
            self.level.add(rec["level"])
            self.trange.add(rec["trange"])
            self.var.add(rec["var"])
            if self.datemin is None or self.datemin > rec["datemin"]:
                self.datemin = rec["datemin"]
            if self.datemax is None or self.datemax < rec["datemax"]:
                self.datemax = rec["datemax"]

    def to_dict(self):
        def level_key(l):
            return tuple((str(x) if x is not None else "") for x in l)
        def trange_key(t):
            return tuple((str(x) if x is not None else "") for x in t)
        return {
            "ana_id": sorted(self.ana_id),
            "rep_memo": sorted(self.rep_memo),
            "level": [(x, dballe.describe_level(*x)) for x in sorted(self.level, key=level_key)],
            "trange": [(x, dballe.describe_trange(*x)) for x in sorted(self.trange, key=trange_key)],
            "var": sorted(self.var),
            "datemin": self.datemin.strftime("%Y-%m-%d %H:%M:%S") if self.datemin is not None else None,
            "datemax": self.datemax.strftime("%Y-%m-%d %H:%M:%S") if self.datemin is not None else None,
        }


class Session:
    def __init__(self, db_url):
        self.loop = asyncio.get_event_loop()
        self.executor = concurrent.futures.ThreadPoolExecutor(max_workers=2)
        self.db_url = db_url
        self.db = dballe.DB.connect_from_url(self.db_url)
        self.filter = Filter()
        self.summary = None

    async def set_filter(self, flt):
        log.debug("Session.set_filter")
        self.filter = Filter.from_dict(flt)

    async def refresh_filter(self):
        log.debug("Session.refresh_filter")
        def _refresh_filter():
            try:
                query = self.filter.to_record()
                query["query"] = "details"
                return self.db.query_summary(query)
            except:
                log.exception("Refresh filter failed")
                return []
        records = await self.loop.run_in_executor(self.executor, _refresh_filter)
        self.summary = await self.loop.run_in_executor(self.executor, functools.partial(Summary, records))
        return self.summary.to_dict()

    async def get_data(self, limit=20):
        log.debug("Session.get_data")
        def _get_data():
            query = self.filter.to_record()
            if limit is not None:
                query["limit"] = limit
            res = []
            for rec in self.db.query_data(query):
                res.append([
                    rec["rep_memo"],
                    rec["ana_id"],
                    rec["var"],
                    rec["level"],
                    rec["trange"],
                    rec["datetime"].strftime("%Y-%m-%d %H:%M:%S"),
                    rec[rec["var"]],
                ])
            return res
        records = await self.loop.run_in_executor(self.executor, _get_data)
        return records
