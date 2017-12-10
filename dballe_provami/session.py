import dballe
import asyncio
from collections import defaultdict
import concurrent.futures
import functools
import logging

log = logging.getLogger(__name__)


class Summary:
    def __init__(self, records, choices, datemin, datemax):
        self.records = records
        self.choices = choices
        self.datemin = datemin
        self.datemax = datemax

    @classmethod
    def create(cls, records):
        log.debug("Summary.update")
        choices = defaultdict(set)
        datemin = None
        datemax = None
        for rec in records:
            choices["ana_id"].add(rec["ana_id"])
            choices["rep_memo"].add(rec["rep_memo"])
            choices["level"].add(rec["level"])
            choices["trange"].add(rec["trange"])
            choices["var"].add(rec["var"])
            if datemin is None or datemin > rec["datemin"]:
                datemin = rec["datemin"]
            if datemax is None or datemax < rec["datemax"]:
                datemax = rec["datemax"]
        return cls(records, choices, datemin, datemax)

    def to_dict(self):
        return {
            "choices": { k: sorted(v) for k, v in self.choices.items() },
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
        self.summary = await self.loop.run_in_executor(self.executor, functools.partial(Summary.create, records))
        return self.summary.to_dict()
