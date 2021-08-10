import os
common = Split("""operators.cc
	       """)
tools = dict()
tools['test.cc'] = 'test'
tools['pmap_to_class.cc'] = 'pmap_to_class'
tools['pcap_chello.cc'] = 'pcap_chello'
tools['pcap_dns.cc'] = 'pcap_dns'
tools['pcap_tcpstream.cc'] = 'pcap_tcpstream'
tools['pcap_randy.cc'] = 'pcap_randy'

libs = Split("""
	     """)
env = Environment(CXX="ccache clang++ -I.. -pthread", CPPFLAGS="-D_FILE_OFFSET_BITS=64 -Wall -g --std=c++17", LIBS=libs, CPPPATH="..")
env['ENV']['TERM'] = 'xterm'
env['ENV']['TARGET'] = 'bin/'

Decider('MD5')
for i in tools:
	env.Program(source = [i] + common, target = tools[i])

#env.Library('ib', common)
