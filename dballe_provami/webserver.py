import os
import json
import tornado.ioloop
import tornado.web
from tornado.web import url
from .wsconnection import Hub
from .webapi import WebAPI, WebAPIError
from .session import Session
import asyncio
import dballe
import logging

log = logging.getLogger(__name__)

class HomeHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")


class Application(tornado.web.Application):
    def __init__(self, db_url, **settings):
        self.loop = asyncio.get_event_loop()
        self.session = Session(db_url)

        urls = [
            url(r"/", HomeHandler, name="home"),
        ]

        self.webapi = WebAPI(self.session)

        self.ws_hub = Hub(self)
        urls.extend(self.ws_hub.router.urls)

        self.ws_hub.subscribe({"channel": "api"}, self.on_api)
        self.ws_hub.subscribe({"channel": "events"}, self.on_event)

        settings.setdefault("static_path", os.path.join(os.path.dirname(__file__), "static"))
        settings.setdefault("template_path", os.path.join(os.path.dirname(__file__), "templates"))
        #settings.setdefault("cookie_secret", "random string")
        #settings.setdefault("xsrf_cookies", True)

        super().__init__(urls, **settings)

    async def async_setup(self):
        log.debug("Async setup")
        options = await self.session.refresh_filter()
        log.debug("Got options", options)
        self.ws_hub.broadcast({"channel": "events", "type": "new_filter", "options": options})

    async def on_api(self, conn, payload):
        log.debug("API call: %r", payload)
        payload["response"] = await self.webapi(**payload)
        conn.send(json.dumps(payload))

    async def on_event(self, conn, payload):
        """
        WebAPI frontend for WebSocket function calls
        """
        log.debug("Event: %r", payload)
        evt_type = payload.get("type")
        if evt_type is None:
            log.warn("Received event without type: %r", payload)
            return
        handler = getattr(self, "on_event_" + evt_type, None)
        if handler is None:
            log.warn("Received event of unknown type: %r", payload)
            return
        await handler(conn, payload)
