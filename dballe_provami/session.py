import dballe
import asyncio
from collections import defaultdict
import concurrent.futures
import functools
import logging

log = logging.getLogger(__name__)


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
        return {
            "choices": {
                "ana_id": sorted(self.ana_id),
                "rep_memo": sorted(self.rep_memo),
                "level": [(x, dballe.describe_level(*x)) for x in sorted(self.level)],
                "trange": [(x, dballe.describe_trange(*x)) for x in sorted(self.trange)],
                "var": sorted(self.var),
            },
            "datemin": self.datemin.strftime("%Y-%m-%d %H:%M:%S"),
            "datemax": self.datemax.strftime("%Y-%m-%d %H:%M:%S"),
        }


class Session:
    def __init__(self, db_url):
        self.loop = asyncio.get_event_loop()
        self.executor = concurrent.futures.ThreadPoolExecutor(max_workers=2)
        self.db_url = db_url
        self.db = dballe.DB.connect_from_url(self.db_url)
        self.filter = dballe.Record()
        self.summary = None

    async def refresh_filter(self):
        log.debug("Session.refresh_filter")
        def _refresh_filter():
            query = self.filter.copy()
            query["query"] = "details"
            return [rec for rec in self.db.query_summary(query)]
        records = await self.loop.run_in_executor(self.executor, _refresh_filter)
        self.summary = await self.loop.run_in_executor(self.executor, functools.partial(Summary, records))
        return self.summary.to_dict()
