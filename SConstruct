import os
import string
# Edit this to suit your application
defines = ['UNIX', 'HAVE_SYS_TYPES_H', 'HAVE_SYS_STAT_H', 'HAVE_UNISTD_H', 'ILUT_USE_OPENGL']
include_path = ['/usr/local/include/colladadom', '/usr/local/include/colladadom/1.4']
cc_flags = ['-Wall', '-Wno-unknown-pragmas']
libs = ['GL', 'GLU', 'xml2', 'pcre', 'pcrecpp', 'minizip', 'boost_system', 'boost_filesystem', 'IL', 'ILU', 'ILUT', 'collada14dom']

debug_cc_flags = ['-g']
release_cc_flags = ['-O2']
debug_libs = []
release_libs = []
subdirs = ['compat/']
prog_name = 'pcs2'

release = Environment()
release.Append(CPPPATH=include_path)
release.Append(CPPDEFINES=defines)
release.Append(LIBS=libs)
release.Append(CCFLAGS=cc_flags)

debug = release.Clone()

release.BuildDir('release/', './')
release.ParseConfig('wx-config --cppflags --libs --gl-libs')
release.Append(LIBS=release_libs)
release.Append(CPPDEFINES=['NDEBUG'])
release.Append(CCFLAGS=release_cc_flags)
sources = Glob("release/*.cpp") + ["release/GLee.c"]
for dir in subdirs:
  sources = sources + Glob('release/' + dir + '*.cpp');
pcs2 = release.Program(prog_name, sources)

debug.BuildDir('debug/', './')
debug.ParseConfig('wx-config --cppflags --libs --gl-libs --debug')
debug.Append(CPPDEFINES=['_DEBUG'])
debug.Append(LIBS=debug_libs)
debug.Append(CCFLAGS=debug_cc_flags)
sources = Glob("debug/*.cpp") + ["debug/GLee.c"]
for dir in subdirs:
  sources = sources + Glob('debug/' + dir + '*.cpp');
pcs2d = debug.Program(prog_name + 'd', sources)
Default(pcs2d)

# Set versions.o to depend on everything else so versions.cpp is always
# rebuilt when linking.
# Taken from http://www.scons.org/wiki/BuildNumberProcessing
from SCons.Node import NodeList
def MakeVersionDeps(env, target, match='versions.'):
  # TODO add error handling/reporting
  # isinstance(target,NodeList) is needed for scons > 0.96
  if type(target) == type([]) or isinstance(target,NodeList):
    target = target[0]
  version = [child for child in target.children() if str(child).find(match) != -1]
  others = [child for child in target.children() if str(child).find(match) == -1]
  if version:
    env.Depends(version, others)

MakeVersionDeps(debug, pcs2d)
MakeVersionDeps(release, pcs2)

