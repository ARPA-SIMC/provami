(function($) {
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Strict_mode
"use strict";

class Provami extends provami.EventsBase
{
    constructor() {
        super();
        var self = this;
        // self.events.foo = provami.Event(self);
        self.conn = window.ws_connection;
        //self.conn.on("message", self._on_message);
    }

    /*
    _on_message(msg) {
        var self = this;
        var parsed = $.parseJSON(msg.data);
        // console.log("Message:", parsed);
        if (parsed.channel == "api")
        {
          // console.log("provami api message", parsed.response);
          if (parsed["function"] == "ping")
            $("#test").append($("<pre>").text("PING:" + JSON.stringify(parsed)));
          else if (parsed["function"] == "async_ping")
            $("#test").append($("<pre>").text("ASYNC PING:" + JSON.stringify(parsed)));
        }
        else if (parsed.channel == "events")
        {
          if (parsed.type == "new_filter")
            $("#test").append($("<pre>").text("FILTER OPTIONS: " + JSON.stringify(parsed.options)));
        }
        else if (parsed.channel == "provami:updates")
        {
          console.log("Update", parsed);
        }
    }
    */

    _get(name, args) {
        var self = this;
        return new Promise((resolve, reject) => {
            $.ajax({
                url: "/api/1.0/" + name,
                data: args,
                dataType: "json",
                success: (data, textStatus, jqXHR) => {
                    resolve(data);
                },
                error: (jqXHR, textStatus, errorThrown) => {
                    console.warn("Response to", name, jqXHR, textStatus, errorThrown);
                    reject(errorThrown);
                },
            });
        });
    }

    async ping() {
        return await this._get("ping", {});
    }

    async async_ping() {
        return await this._get("async_ping", {});
    }

    async get_filter_stats() {
        return await this._get("get_filter_stats", {});
    }
}

window.provami = $.extend(window.provami || {}, {
    Provami: Provami,
});

})(jQuery);
