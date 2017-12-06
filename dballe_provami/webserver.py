import os
import json
import tornado.ioloop
import tornado.web
from tornado.web import url
from .wsconnection import Hub
from .webapi import WebAPI, WebAPIError
import dballe
import logging

log = logging.getLogger(__name__)

class HomeHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")


class Application(tornado.web.Application):
    def __init__(self, db_url, **settings):
        self.db_url = db_url
        self.db = dballe.DB.connect_from_url(self.db_url)

        urls = [
            url(r"/", HomeHandler, name="home"),
        ]

        self.webapi = WebAPI(self.db)

        self.ws_hub = Hub(self)
        urls.extend(self.ws_hub.router.urls)

        self.ws_hub.subscribe({"channel": "api"}, self.on_client_request)

        settings.setdefault("static_path", os.path.join(os.path.dirname(__file__), "static"))
        settings.setdefault("template_path", os.path.join(os.path.dirname(__file__), "templates"))
        #settings.setdefault("cookie_secret", "random string")
        #settings.setdefault("xsrf_cookies", True)

        super().__init__(urls, **settings)

    async def on_client_request(self, conn, payload):
        """
        WebAPI frontend for WebSocket function calls
        """
        payload["response"] = await self.webapi(**payload)
        conn.send(json.dumps(payload))

