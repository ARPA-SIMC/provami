import time
import logging
import asyncio
import csv
import dballe
import io

log = logging.getLogger(__name__)


class WebAPIError(Exception):
    def __init__(self, code, msg):
        super().__init__(msg)
        self.code = code


class WebAPI:
    """
    Backend-independent functions exposed via REST or WebSocket APIs
    """
    def __init__(self, db):
        self.db = db

    async def __call__(self, function=None, **kw):
        """
        Call a web API function by name and keyword arguments
        """
        log.debug("API call %s %r", function, kw)
        if function is None:
            log.debug("API call %s %s: function is missing", function, kw)
            raise gen.Return(None)
        f = getattr(self, "do_" + function)
        if f is None:
            log.debug("API call %s %s: function not found", function, kw)
            raise gen.Return(None)
        if asyncio.iscoroutinefunction(f):
            res = await f(**kw)
        else:
            res = f(**kw)
        log.debug("API call %s %r result %r", function, kw, res)
        res["time"] = time.time()
        return res

    def do_ping(self, **kw):
        return {
            "pong": True,
        }

    async def do_async_ping(self, **kw):
        return {
            "pong": True,
        }

    async def do_query_summary(self, **kw):
        query = dballe.Record()
        query["query"] = "details"
        out = io.StringIO()
        writer = csv.writer(out)
        cur = self.db.query_summary(query)
        for rec in cur:
            writer.writerow([
                rec["ana_id"], rec["rep_memo"], rec["level"], rec["trange"], rec["var"],
                rec["datemin"], rec["datemax"], rec["context_id"]])
        return {
            "summary": out.getvalue(),
        }
