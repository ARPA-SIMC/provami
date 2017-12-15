import tornado.testing
import unittest
from dballe_provami.webapi import WebAPI
from dballe_provami.session import Session
import dballe
import datetime
from dballe_provami.unittest import async_test, AsyncTestCase
from unittest import mock

class TestWebAPIMixin:
    def setUp(self):
        super().setUp()
        self.session = Session("sqlite::memory:")
        self.session.db.reset()
        self.api = WebAPI(self.session)


class TestPing(TestWebAPIMixin, AsyncTestCase):
    @async_test
    async def test_ping(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("ping")
            self.assertEqual(res, { "time": 100, "pong": True })

    @async_test
    async def test_async_ping(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("ping")
            self.assertEqual(res, { "time": 100, "pong": True })


class TestEmpty(TestWebAPIMixin, AsyncTestCase):
    @async_test
    async def test_get_filter_stats(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("get_filter_stats")
            self.assertEqual(res, { "time": 100, "empty": True })

    @async_test
    async def test_get_data(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("get_data")
            self.assertEqual(res, { "time": 100, "rows": [] })


class TestBasic(TestWebAPIMixin, AsyncTestCase):
    def setUp(self):
        super().setUp()
        self.data = dballe.Record(
                lat=12.34560, lon=76.54320,
                mobile=0,
                datetime=datetime.datetime(1945, 4, 25, 8, 0, 0),
                level=(10, 11, 15, 22),
                trange=(20, 111, 222),
                rep_memo="synop",
                B01011="Hey Hey!!",
                B01012=500)
        self.ids = self.session.db.insert_data(self.data, False, True)

    @async_test
    async def test_get_filter_stats(self):
        await self.session.refresh_filter()
        with mock.patch("time.time", return_value=100):
            res = await self.api("get_filter_stats")
            self.maxDiff = None
            self.assertEqual(res, { "time": 100,
                "available": {
                    'ana_id': [1],
                    'level': [((10, 11, 15, 22), 'Layer from [10 11] to [15 22]')],
                    'trange': [((20, 111, 222), '20 111 222')],
                    'rep_memo': ['synop'],
                    'var': ['B01012'],
                    'datemax': '1945-04-25 08:00:00',
                    'datemin': '1945-04-25 08:00:00',
                },
                "current": {
                    'ana_id': None,
                    'datemax': None,
                    'datemin': None,
                    'level': None,
                    'rep_memo': None,
                    'trange': None,
                    'var': None
                },
            })

    @async_test
    async def test_get_data(self):
        with mock.patch("time.time", return_value=100):
            res = await self.api("get_data")
            self.assertEqual(res, { "time": 100,
                "rows": [
                    ['synop', 1, 'B01011', (10, 11, 15, 22), (20, 111, 222), '1945-04-25 08:00:00', 'Hey Hey!!'],
                    ['synop', 1, 'B01012', (10, 11, 15, 22), (20, 111, 222), '1945-04-25 08:00:00', 500],
                ],
            })
