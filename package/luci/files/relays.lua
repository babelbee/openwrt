--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: leds.lua 7037 2011-05-09 12:51:01Z jow $
]]--
m = Map("relays", translate("Relay Configuration"), translate("Customizes the behaviour of the device relays."))

local fs   = require "nixio.fs"
local util = require "nixio.util"

s = m:section(TypedSection, "relay", "")
s.anonymous = true
s.addremove = false

function s.parse(self, ...)
	TypedSection.parse(self, ...)
	os.execute("/etc/init.d/relay enable")
end


sysfs = s:option(DummyValue, "name", translate("Relay Name"))

new = s:option(ListValue, "new", translate("New State"))

new:value("on", translate("On"))
new:value("off", translate("Off"))

boot = s:option(ListValue, "boot", translate("Boot State"))

boot:value("last", translate("Last"))
boot:value("on", translate("On"))
boot:value("off", translate("Off"))

return m
