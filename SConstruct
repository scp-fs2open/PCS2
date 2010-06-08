import os
import string
# Edit this to suit your application
defines = ['UNIX', 'HAVE_SYS_TYPES_H', 'HAVE_SYS_STAT_H', 'HAVE_UNISTD_H', 'ILUT_USE_OPENGL']
include_path = ['/usr/local/include/colladadom', '/usr/local/include/colladadom/1.4']
cc_flags = ['-Wno-deprecated']
libs = ['GL', 'GLU', 'xml2', 'pcre', 'pcrecpp', 'minizip', 'boost_system', 'boost_filesystem', 'IL', 'ILU', 'ILUT', 'collada14dom']

debug_cc_flags = ['-g']
release_cc_flags = ['-O2']
debug_libs = []
release_libs = []
subdirs = ['compat/']
prog_name = 'pcs2'

pcs2 = Environment()
pcs2.Append(CPPPATH=include_path)
pcs2.Append(CPPDEFINES=defines)
pcs2.Append(LIBS=libs)
pcs2.Append(CCFLAGS=cc_flags)

pcs2d = pcs2.Clone()

pcs2.BuildDir('release/', './')
pcs2.ParseConfig('wx-config --cppflags --libs --gl-libs')
pcs2.Append(LIBS=release_libs)
pcs2.Append(CPPDEFINES=['NDEBUG'])
sources = Glob("release/*.cpp") + ["release/GLee.c"]
for dir in subdirs:
        sources = sources + Glob('release/' + dir + '*.cpp');
pcs2.Program(prog_name, sources, CCFLAGS=release_cc_flags)

pcs2d.BuildDir('debug/', './')
pcs2d.ParseConfig('wx-config --cppflags --libs --gl-libs --debug')
pcs2d.Append(CPPDEFINES=['_DEBUG'])
pcs2d.Append(LIBS=debug_libs)
sources = Glob("debug/*.cpp") + ["debug/GLee.c"]
for dir in subdirs:
        sources = sources + Glob('debug/' + dir + '*.cpp');
Default(pcs2d.Program(prog_name + 'd', sources, CCFLAGS=debug_cc_flags))

