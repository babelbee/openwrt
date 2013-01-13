--[[

Luci statistics - exec plugin diagram definition
(c) 2008 Freifunk Leipzig / Jo-Philipp Wich <xm@leipzig.freifunk.net>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

$Id: power.lua 6810 2011-01-29 03:33:48Z jow $

]]--

module("luci.statistics.rrdtool.definitions.power", package.seeall)

function rrdargs( graph, plugin, plugin_instance, dtype )
	return {
		{
			title = "%H: Voltage",
			vlabel = "V",
			rrdopts = {
				"-y","1:1",
			},
			number_format = "%3.2lf V",
			data = {
				types = { "voltage" },
				instances = {
					voltage = { "L1", "L2", "L3" }
				},
				options = {
					L1 = { color = "0000ff" },
					L2 = { color = "00ff00" },
					L3 = { color = "ff0000" },
					voltage = {
						title = "%di",
						noarea=true,
						overlay=true
					}
				}
			}
		},
		{
			title = "%H: Current",
			vlabel = "A",
			number_format = "%2.3lf A",
			data = {
				types = { "current" },
				instances = {
					current = { "L1", "L2", "L3", "N" }
				},
				options = {
					current = {
						title = "%di",
						noarea=true,
						overlay=true
					}
				}
			}
		},
		{
			title = "%H: Total Power",
			vlabel = "W",
			number_format = "%5.0lf W",
			data = {
				types = { "gauge" },
				instances = {
					gauge = { "Lsum" }
				},
				options = {
					gauge = {
						title = "%di",
					}
				}
			}
		},
		{
			title = "%H: Power",
			vlabel = "W",
			number_format = "%5.0lf W",
			data = {
				types = { "gauge" },
				instances = {
					gauge = { "L1", "L2", "L3" }
				},
				options = {
					gauge = {
						title = "%di",
						noarea=true,
						overlay=true
					}
				}
			}
		},
		{
			title = "%H: Apparent Power",
			vlabel = "VA",
			number_format = "%5.0lf VA",
			data = {
				types = { "gauge" },
				instances = {
					gauge = { "L1a", "L2a", "L3a" }
				},
				options = {
					gauge = {
						title = "%di",
						noarea=true,
						overlay=true
					}
				}
			}
		},
		{
			title = "%H: Reactive Power",
			vlabel = "VA",
			number_format = "%5.0lf VA",
			data = {
				types = { "gauge" },
				instances = {
					gauge = { "L1r", "L2r", "L3r" }
				},
				options = {
					gauge = {
						title = "%di",
						noarea=true,
						overlay=true
					}
				}
			}
		},
		{
			title = "%H: Frequency",
			vlabel = "Hz",
			number_format = "%2.3lf Hz",
			rrdopts = {
				"-y","0.1:1",
			},
			data = {
				types = { "frequency" },
				sources = { 
					frequency = {"frequency" },
				},
				options = {
					frequency = {
						title = "%di",
					}
				}
			}
		},
	}
end
