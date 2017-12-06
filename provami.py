#!/usr/bin/env python3
import os
import os.path
import sys
import webbrowser
from dballe.provami.webserver import Application
from tornado.options import define, options
from tornado.platform.asyncio import AsyncIOMainLoop
import asyncio
AsyncIOMainLoop().install()

class Provami:
    def __init__(self):
        self.db_url = options.db
        self.web_host = options.web_host
        self.web_port = options.web_port

    def setup(self):
        # Set up web server
        self.application = Application(self.db_url)
        self.application.listen(self.web_port, address=self.web_host)


def main():
    define("web_host", default="127.0.0.1", help="listening hostname for web interface")
    define("web_port", type=int, default=8000, help="listening port for web interface")
    define("db", type=str, metavar="dballe_url", default=os.environ.get("DBA_DB"), help="DB-All.e database to connect to")
    options.parse_command_line()

    if not options.db:
        print("Please provide a database URL with --db", file=sys.stderr)
        options.print_help()
        return 1

    provami = Provami()
    provami.setup()
    webbrowser.open("http://{}:{}/".format(options.web_host, options.web_port))

    asyncio.get_event_loop().run_forever()


if __name__ == "__main__":
    sys.exit(main())
