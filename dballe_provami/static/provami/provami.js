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
                method: "GET",
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

    _post(name, args) {
        var self = this;
        return new Promise((resolve, reject) => {
            $.ajax({
                url: "/api/1.0/" + name,
                method: "POST",
                data: JSON.stringify(args),
                processData: false,
                dataType: "json",
                contentType: "application/json",
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

    async set_filter(filter) {
        return await this._post("set_filter", {filter: filter});
    }
}

class FilterField
{
    constructor(provami, name)
    {
        this.provami = provami
        this.name = name;
        this.row = $("#filter-" + this.name);
        this.add = $("#filter-add-" + this.name);
        this.add.click(evt => { this.row.show(); });
        this.remove = this.row.find("button.remove");
        this.value = null;
        this.remove.click(evt => { this.provami.set_filter(this.name, null).then(); });
    }

    _get_option(o) {
        if (o instanceof Array)
            return o;
        else
            return [o, o];
    }
}

class FilterFieldChoices extends FilterField
{
    constructor(provami, name)
    {
        super(provami, name);
        this.field = $("#filter-field-" + name);
        this.field.change(evt => { this.provami.set_filter(this.name, this.field.val()).then(); });
    }

    _set_forced(value)
    {
        // Only one available option, mark it as hardcoded
        value = this._get_option(value);
        this.value = null;
        this.add.hide();
        this.remove.hide();
        this.row.find("td.value span.value").text(value[1]).show();
        this.field.hide();
    }

    _set_multi(options)
    {
        // Multiple available options
        this.value = null;

        // Fill the <option> list in the <select> field
        this.field.empty();
        this.field.append("<option value='' selected>-------</option>");
        for (var i = 0; i < options.length; ++i)
        {
            var o = this._get_option(options[i]);
            var opt = $("<option>").attr("value", o[0]).text(o[1]);
            this.field.append(opt);
        }

        this.remove.hide();
        this.row.find("td.value span.value").hide();
        this.field.show();
        this.add.show();
    }

    _set_chosen(value)
    {
        // Chosen: show the choice
        value = this._get_option(value);
        this.value = value[0];
        this.remove.show();
        this.row.find("td.value span.value").text(value[1]).show();
        this.field.hide();
        this.add.hide();
    }

    update(stats)
    {
        var current = stats.current[this.name];
        var options = stats.available[this.name];
        console.log("Update field", this.name, current, options);
        if (current == null)
        {
            // Not currently chosen
            if (options.length == 1)
                this._set_forced(options[0]);
            else
                this._set_multi(options);
        } else
            this._set_chosen(current);
    }
}

class Provami
{
    constructor()
    {
        $("#filter_fields").attr("disabled", true);
        $("#filter_update").attr("disabled", true);
        $("#filter").submit(() => { return false; });
        //$("#filter").change(() => { $("#filter_update").attr("disabled", false); });
        $("#filter_update").click(evt => { this.submit_filter(); });
        this.server = new window.provami.Server();
        this.server.on("new_filter", msg => { this.update_filter().then(); });
        this.fields = [
            new FilterFieldChoices(this, "rep_memo"),
            new FilterFieldChoices(this, "var"), // TODO: rename in varcode
            new FilterFieldChoices(this, "level"),
            new FilterFieldChoices(this, "trange"),
        ];
    }

    async set_filter(field, value)
    {
        console.log("Set field", field, value);
        var filter = {};
        // Collect the current filter values
        $.each(this.fields, (idx, field) => {
            if (field.value == null) return;
            filter[field.name] = field.value;
        });
        // Patch with this new value
        if (value == null)
            delete filter[field];
        else
            filter[field] = value;

        console.log("Set new filter", filter);
        await this.server.set_filter(filter);
        await this.update_all();
    }

    async update_filter()
    {
        var stats = await this.server.get_filter_stats();
        console.log("New filter stats:", stats);

        $.each(this.fields, (idx, el) => {
            el.update(stats);
        });

        /*
        if (stats.initializing)
        {
            $("#filter tfoot").hide();
            //$("#filter tbody.inner").replaceWith("<tr><td colspan='2'>Initializing</td></tr>");
            //$("#filter_fields").attr("disabled", true);
        } else {
            //$("#filter tbody.inner").replaceWith("<tr><td colspan='2'>Initializing</td></tr>");\n"
            //this._set_options("#filter-repmemo", "rep_memo", stats);
            //this._set_options("#filter-var", "var", stats);
            //this._set_options("#filter-level", "level", stats);
            //this._set_options("#filter-trange", "trange", stats);
            //$("#filter_fields").attr("disabled", false);
            $("#filter tfoot").hide();
        }
        */

        $("#filter-new-button").attr("disabled", stats.initializing);
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
