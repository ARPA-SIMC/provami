import tornado.testing
import unittest
from dballe_provami.webapi import WebAPI
from dballe_provami.session import Session
from unittest import mock

class TestWebAPIMixin:
    def setUp(self):
        super().setUp()
        self.session = Session("sqlite::memory:")
        self.api = WebAPI(self.session)


class TestPing(TestWebAPIMixin, tornado.testing.AsyncTestCase):
    @tornado.testing.gen_test
    async def test_ping(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("ping")
            self.assertEqual(res, { "time": 100, "pong": True })

    @tornado.testing.gen_test
    async def test_async_ping(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("ping")
            self.assertEqual(res, { "time": 100, "pong": True })


class TestEmpty(TestWebAPIMixin, tornado.testing.AsyncTestCase):
    @tornado.testing.gen_test
    async def test_get_filter_stats(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("get_filter_stats")
            self.assertEqual(res, { "time": 100, "empty": True })

    @tornado.testing.gen_test
    async def test_get_data(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("get_data")
            self.assertEqual(res, { "time": 100, "rows": [] })
