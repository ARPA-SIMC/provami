import os
import logging
import tornado.ioloop
import tornado.web
from tornado.web import url

log = logging.getLogger(__name__)

class HomeHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")


class Application(tornado.web.Application):
    def __init__(self, db_url, **settings):
        self.db_url = db_url

        urls = [
            url(r"/", HomeHandler, name="home"),
        ]

        settings.setdefault("static_path", os.path.join(os.path.dirname(__file__), "static"))
        settings.setdefault("template_path", os.path.join(os.path.dirname(__file__), "templates"))
        #settings.setdefault("cookie_secret", "random string")
        #settings.setdefault("xsrf_cookies", True)

        super().__init__(urls, **settings)
