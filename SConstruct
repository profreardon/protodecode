import os
common = Split("""operators.cc
	       """)
tools = dict()
tools['test.cc'] = 'test'

libs = Split("""
	     """)
env = Environment(CXX="ccache clang++ -I.. -pthread", 		  CPPFLAGS="-D_FILE_OFFSET_BITS=64 -Wall -g --std=c++17", LIBS=libs, CPPPATH="..")
env['ENV']['TERM'] = 'xterm'
env['ENV']['TARGET'] = 'bin/'

Decider('MD5')
for i in tools:
	env.Program(source = [i] + common, target = tools[i])

#env.Library('ib', common)