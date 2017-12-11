/*

 * Encapsulate a sockjs connection and keep it open

The API class is added as window.provami.Connection, and the examples below
assume that the base template instantiates it and makes it accessible at
window.ws_connection, with something like:

    window.ws_connection = new provami.Connection("url");

Reconnection is automatically handled with increasing timeouts.

TODO: Outbound messages are queued when the connection is close, and resent
      once it is opened again. (TODO: verify if it's possible to know that a
      messages has actually been sent and can be removed from the queue)

 */
(function($) {
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Strict_mode
"use strict";

/**
 * JQuery-style event management.
 *
 * An event object has on, off and trigger methods similar to JQuery's ones,
 * except that the event name is not provided among the function arguments.
 *
 * cb_this is the object that will be used as the 'this' value when calling
 * callbacks.
 */
class Event
{
    constructor(cb_this) {
        var self = this;
        self.cb_this = cb_this;
        self.callbacks = [];
    }

    on(cb) {
        var self = this;
        self.callbacks.push(cb);
    }
    off(cb) {
        var self = this;
        var cur_idx = self.callbacks.indexOf(cb);
        if (cur_idx != -1)
            self.callbacks = self.callbacks.splice(cur_idx, 1)
    }
    /// Trigger an event. All the arguments will be passed to the event
    /// callback functions
    trigger() {
        var self = this;
        for (var cbidx in self.callbacks)
        {
            var cb = self.callbacks[cbidx];
            cb.apply(self.cb_this, arguments);
        }
    }
}

/**
 * Base class for objects that handle JQuery-style events.
 *
 * This implements on() and off() functions that 
 */
class EventsBase
{
    constructor() {
        var self = this;
        self.events = {}
    }

    // Add a callback to an event
    on(name, cb) {
        var self = this;
        var evt = self.events[name];
        if (!evt) throw "Event not found: " + name;
        evt.on(cb);
    }

    // Remove a callback from an event
    off(name, cb) {
        var self = this;
        var evt = self.events[name];
        if (!evt) throw "Event not found: " + name;
        evt.off(cb);
    }
}


/**
 * Manage a Websocket/SockJS connection, reconnecting when it closes.
 */
class Connection extends EventsBase
{
    constructor(websocket_url) {
        super();
        var self = this;
        self.events.connect = new Event(self);
        self.events.message = new Event(self);
        self.events.disconnect = new Event(self);
        self.url = websocket_url;
        self.is_open = false;
        self.send_queue = [];
        self.ws = null;
        self.connect_timeouts = [200, 500, 1000, 2000, 3000, 3000];
        self.cur_connect_timeout = 0;
        self._reconnect();
    }

    /**
     * Send a message to the server.
     *
     * message is an arbitrary javascript data structure that will be encoded
     * to JSON and sent to the server.
     *
     * If the connection is offline, the message will be enqueued for when it
     * will become available again. Since there is no notification of
     * successful reception on send, this could lead to duplicate or lost
     * messages in case the connection is lost during a send.
     */
    send(message) {
        var self = this;
        var encoded = JSON.stringify(message);
        self.send_queue.push(encoded);
        self._flush_send_queue();
    }
    _flush_send_queue() {
        var self = this;
        if (!self.is_open) return;
        while (self.send_queue.length)
        {
            self.ws.send(self.send_queue[0]);
            self.send_queue.shift()
        }
    }
    // Open a new connection
    _reconnect() {
        var self = this;
        console.log(self.url, "Sockjs connection attempted");
        //console.log("Connecting to " + self.url + "...");
        if (self.ws) self.ws.close();
        self.ws = new SockJS(self.url);
        self.ws.onopen = function() { self._on_open(); }
        self.ws.onmessage = function(evt) { self._on_message(evt); }
        self.ws.onclose = function() { self._on_close(); }
    }
    _on_open() {
        var self = this;
        console.log(self.url, "Sockjs connection established");
        //console.log("Connection.on_open " + self.url);
        self.is_open = true;
        self.cur_connect_timeout = 0;
        self._flush_send_queue();
        self.events.connect.trigger();
    }
    _on_message(evt) {
        var self = this;
        //console.log("Connection.on_message", evt);
        evt.payload = JSON.parse(evt.data);
        self.events.message.trigger(evt);
    }
    _on_close() {
        var self = this;
        console.log(self.url, "Sockjs connection lost");
        self.is_open = false;
        self.ws.onopen = null;
        self.ws.onmessage = null;
        self.ws.onclose = null;
        self.ws.close();
        self.ws = null;
        self.events.disconnect.trigger();
        setTimeout(function() { self._reconnect(); }, self.connect_timeouts[self.cur_connect_timeout]);
        self.cur_connect_timeout = (self.cur_connect_timeout + 1) % self.connect_timeouts.length;
    }
}

window.provami = $.extend(window.provami || {}, {
    Event: Event,
    EventsBase: EventsBase,
    Connection: Connection,
});

})(jQuery);
