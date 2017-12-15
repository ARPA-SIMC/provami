(function($) {
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Strict_mode
"use strict";

class Server extends provami.EventsBase
{
    constructor() {
        super();
        var self = this;
        self.events.new_filter = new provami.Event(self);
        self.conn = window.ws_connection;
        self.conn.on("message", msg => { self._on_message(msg); } );
    }

    _on_message(msg) {
        var self = this;
        var parsed = $.parseJSON(msg.data);
        // console.log("Message:", parsed);
        if (parsed.channel == "events")
        {
          if (parsed.type == "new_filter")
            self.events.new_filter.trigger(parsed);
        }
    }

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

    async get_data() {
        return await this._get("get_data", {});
    }
}

class Provami
{
    constructor()
    {
        $("#filter_fields").attr("disabled", true);
        $("#filter").submit(() => { return false; });
        this.server = new window.provami.Server();
        this.server.on("new_filter", msg => { this.update_filter().then(); });
    }

    _set_options(field, name, stats)
    {
        var options = stats.available[name];
        var current = stats.current[name];
        console.log(field, name, stats, options, current)

        field = $(field);
        field.empty();
        var no_choice = $("<option>").attr("value", null).text("-----");
        if (current == null)
            no_choice.attr("selected", true);
        field.append(no_choice);

        for (var i = 0; i < options.length; ++i)
        {
            var value, text;
            if (options[i] instanceof Array)
            {
                value = options[i][0];
                text = options[i][1];
            } else {
                value = options[i];
                text = options[i];
            }
            var opt = $("<option>").attr("value", value).text(text);
            if (value == current)
                opt.attr("selected", true);
            field.append(opt);
        }
    }

    async update_filter()
    {
        var stats = await this.server.get_filter_stats();

        if (stats.empty)
        {
            $("#filter_fields").attr("disabled", true);
        } else {
            this._set_options("#filter-repmemo", "rep_memo", stats);
            this._set_options("#filter-var", "var", stats);
            this._set_options("#filter-level", "level", stats);
            this._set_options("#filter-trange", "trange", stats);
            $("#filter_fields").attr("disabled", false);
        }
    }

    async update_data()
    {
        var data = await this.server.get_data();

        var tbody = $("#data tbody");
        tbody.empty();

        for (var i = 0; i < data.rows.length; ++i)
        {
            var row = data.rows[i];
            var tr = $("<tr>");
            tr.append($("<td>").text(row[0]));
            tr.append($("<td>").text(row[1]));
            tr.append($("<td>").text(row[2]));
            tr.append($("<td>").text(row[3]));
            tr.append($("<td>").text(row[4]));
            tr.append($("<td>").text(row[5]));
            tr.append($("<td>").text(row[6]));
            tbody.append(tr);
        }
    }

    async update_all()
    {
        await Promise.all([this.update_filter(), this.update_data()]);
    }
}

window.provami = $.extend(window.provami || {}, {
    Server: Server,
    Provami: Provami,
});

})(jQuery);
