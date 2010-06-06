import os
import string
# Edit this to suit your application
cc_flags = '-g -Wno-deprecated ' + string.strip(os.popen('wx-config --cppflags --debug').read()) + ' -I/usr/local/include/colladadom -I/usr/local/include/colladadom/1.4 -DUNIX -DHAVE_SYS_TYPES_H -DHAVE_SYS_STAT_H -DHAVE_UNISTD_H -DILUT_USE_OPENGL'
ld_flags = '-lopenal -lalut -lGL -lGLU ' + string.strip(os.popen('wx-config --libs --debug gl').read()) + ' ' + string.strip(os.popen('wx-config --libs --debug').read()) + ' -lcollada14dom -lxml2 -lpcre -lpcrecpp -lminizip -lboost_system -lboost_filesystem -lIL -lILU -lILUT'
subdirs = ['compat/']
prog_name = 'pcs2'

# You Shouldn't need to touch any of this
sources = Glob("*.cpp") + ["GLee.c"]
for dir in subdirs:
        sources = sources + Glob(dir + '*.cpp');

Program(prog_name, sources, CCFLAGS=cc_flags, LINKFLAGS=ld_flags)

