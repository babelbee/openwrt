--[[

Copyright 2011 Manuel Munz <freifunk at somakoma dot de>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0
]]--

m = Map("luci_statistics",
	translate("Power Plugin Configuration"),
	translate("The power plugin collects statistics about power usage."))

s = m:section( NamedSection, "collectd_power", "luci_statistics" )

enable = s:option( Flag, "enable", translate("Enable this plugin") )
enable.default = 1

exec_cmdline = s:option( Value, "cmdline", translate("Script") )       
exec_cmdline.default = "/usr/bin/ade7854 collectd"
                                                                          
exec_cmduser = s:option( Value, "cmduser", translate("User") )         
exec_cmduser.default  = "nobody"                                          
exec_cmduser.rmempty  = true                                              
exec_cmduser.optional = true                                              
                                                                                                                                              
exec_cmdgroup = s:option( Value, "cmdgroup", translate("Group") )      
exec_cmdgroup.default  = "nogroup"
exec_cmdgroup.rmempty  = true                                              
exec_cmdgroup.optional = true                                              

return m
