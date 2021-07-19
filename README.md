# protodecode

tool to make specifying protocols and getting fields simpler

way simpler.

# making

use 'scons' to make the project

make pmap_to_class to turn pmap files into actual classes

# tcpstream

use 'pcap_tcpstream <pcapfile>' to split out tcp streams. This will use the
pcapfile name, replacing .pcap with .tcp / .tcplen and put the port-pair in the
file as well. The .tcp will have the tcp stream, and the .tcplen gives how many
bytes were written in each transmission.

