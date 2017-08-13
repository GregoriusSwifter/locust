#!/usr/bin/env python
import mkpy as mk
import glob
import os
import platform
import fnmatch
import shutil

def rglob(pattern, start_dir=None):
	if start_dir is None:
		start_dir = os.getcwd()
	for path, dirs, files in os.walk(os.path.abspath(start_dir)):
		for filename in fnmatch.filter(files, pattern):
			yield os.path.join(path, filename)

root_dir = os.path.dirname(os.path.abspath(__file__))
source_dir = os.path.join(root_dir, 'src')
build_dir = os.path.join(root_dir, 'build')
system = platform.system()

cc = 'clang'
opt_mode = '-O0 -g -DDEBUG -integrated-as'
#opt_mode = '-O3 -integrated-as'
c_mode= '-c -std=c99 -m64 -fno-builtin -target i686-pc-windows-gnu'#-target x86_64-w64-mingw32' # -target x86_64-w64-mingw32' #-target x86_64-w64-mingw32
c_warn_mode = '-fcolor-diagnostics -Weverything -Wno-padded -Wno-unused-function -Wno-unused-macros -Wno-unused-parameter -Wno-pointer-arith -ferror-limit=5 -Wno-gnu '
#if system.startswith('Windows'):
#	c_warn_mode += ' -Wno-microsoft -Wno-unknown-pragmas -Wno-ignored-attributes -Wno-invalid-token-paste -Wno-int-to-pointer-cast -Wno-unused-value'
c_include_dirs = [os.path.join(root_dir, 'include')]
c_lib_dirs = [os.path.join(root_dir, 'lib')]
c_flags = ''
if system.startswith('FreeBSD'):
	c_include_dirs.append(r'/usr/include')
	c_include_dirs.append(r'/usr/local/include')
	c_libs = []
elif system.startswith('Windows'):
	c_include_dirs.append(os.path.join(root_dir, 'include'))
	#c_flags += r' -isystem E:\src\mingw-w64-v2.0.8\mingw-w64-crt'
	#c_include_dirs.append(r'E:\src\mingw-w64-v2.0.8\mingw-w64-crt')
	
	#c_flags += r' -isystem E:\src\msys\opt\windows_64\x86_64-w64-mingw32\include'
	#c_include_dirs.append(r'E:\src\msys\opt\windows_64\x86_64-w64-mingw32\include')
	#c_flags += r' -isystem E:\src\msys\opt\windows_64\x86_64-w64-mingw32\lib\clang\3.3\include '
	#c_include_dirs.append(r'E:\src\msys\opt\windows_64\x86_64-w64-mingw32\lib\clang\3.3\include')
	#c_lib_dirs.append(r'E:\src\msys\opt\windows_64\x86_64-w64-mingw32\lib')

	#c_flags += r' -isystem C:\MinGW64\mingw64\x86_64-w64-mingw32\include '
	#c_include_dirs.append(r'C:\MinGW64\mingw64\x86_64-w64-mingw32\include')
	#c_flags += r' -isystem C:\MinGW64\mingw64\x86_64-w64-mingw32\lib\clang\3.3\include '
	#c_include_dirs.append(r'C:\MinGW64\mingw64\x86_64-w64-mingw32\lib\clang\3.3\include')
	#c_lib_dirs.append(r'C:\MinGW64\mingw64\x86_64-w64-mingw32\lib')
	c_libs = []

if system.startswith('FreeBSD'):
	c_flags += ' -D__TARGET_FREEBSD '
elif system.startswith('Windows'):
	c_flags += ' -D__TARGET_WINDOWS '
elif system.startswith('Linux'):
	c_flags += ' -D__TARGET_LINUX '

c_flags += opt_mode
c_flags += ' '+c_mode
c_flags += ' -I'+' -I'.join(c_include_dirs)
c_flags += ' '+c_warn_mode
c_files = [file for file in rglob('*.c', source_dir)]
c_files_dest = {f: f.replace(source_dir, build_dir)+'.o' for f in c_files }

ll = cc
l_mode = '-m64 -static' # -target x86_64-w64-mingw32
l_flags = '-v '
l_flags += opt_mode
l_flags += ' '+l_mode
l_flags += ' -L'+' -L'.join(c_lib_dirs)

@mk.task
def build():
	compile()
	link()

@mk.task
def rebuild():
	clean()
	build()

@mk.task
def link():
	args = [ll]
	args.extend(l_flags.split())
	args.append('-o'+os.path.join(build_dir, 'client'))
	for file in c_files:
		args.append(c_files_dest[file])
	c_libs_ = ' -l'+' -l'.join(c_libs) if c_libs else ''
	args.extend(c_libs_.split())
	mk.run(*args)
    
@mk.task
def compile():
	to_compile = []
	for file in c_files:
		# print(file)
		if not os.path.exists(c_files_dest[file]):
			# print("Path exists.")
			to_compile.append(file)
			continue
		if os.path.getmtime(file) >= os.path.getmtime(c_files_dest[file]):
			# print("Path old.")
			to_compile.append(file)
			continue

	#compile
	for file in to_compile:
		# print(file + " again")
		args = [cc]
		args.extend(c_flags.split())
		args.append('-o'+c_files_dest[file])
		args.append(file)
		mk.run(*args)
		# print(file + " again!!!")

@mk.task
def run():
	cur_path = os.getcwd()
	os.chdir(build_dir)
	mk.run(os.path.join(build_dir, 'client'))
	os.chdir(cur_path)

@mk.task
def clean():
	c_build_dirs = [os.path.dirname(f.replace(source_dir, build_dir)) for f in c_files]
	mk.rmdirs(build_dir)
	mk.mkdirs(build_dir)
	#shutil.copytree(os.path.join(root_dir, 'data_inline'), build_dir)
	mk.mkdirs(*c_build_dirs)
	#shutil.copytree(os.path.join(root_dir, 'data'), os.path.join(build_dir, 'data'))


if __name__=='__main__':
	mk.build()

