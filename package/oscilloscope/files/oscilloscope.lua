--[[
LuCI - Lua Configuration Interface

Copyright 2008 Steven Barth <steven@midlink.org>
Copyright 2011 Jo-Philipp Wich <xm@subsignal.org>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

$Id: status.lua 9484 2012-11-21 19:29:47Z jow $
]]--

module("luci.controller.admin.oscilloscope", package.seeall)

function index()
	entry({"admin", "status", "realtime", "oscilloscope"}, template("admin_status/oscilloscope"), _("Oscilloscope"), 6).leaf = true
	entry({"admin", "status", "realtime", "oscilloscope_status"}, call("action_oscilloscope")).leaf = true
end

function action_oscilloscope(src1,src2,src3)
	luci.http.prepare_content("application/json")
	src1=src1+0
	src2=src2+0
	src3=src3+0
	local bwc = io.popen("ade7854 oscilloscope " .. src1 .. " " .. src2 .. " " .. src3 .. " 2>/dev/null")
	if bwc then
		luci.http.write("[")
		
		while true do
			local ln = bwc:read("*l")
			if not ln then break end
			luci.http.write(ln)
		end
		luci.http.write("]")
		bwc:close()
	end
end
