import os
import string
# Edit this to suit your application
cc_flags = '-g -Wno-deprecated ' + string.strip(os.popen('wx-config --cppflags').read())
ld_flags = '-lopenal -lalut -lGL -lGLU ' + string.strip(os.popen('wx-config --libs').read())
subdirs = ['compat/']
prog_name = 'pcs2'

# You Shouldn't need to touch any of this
sources = Glob("*.cpp")
for dir in subdirs:
        sources = sources + Glob(dir + '*.cpp');

Program(prog_name, sources, CCFLAGS=cc_flags, LDFLAGS=ld_flags)

