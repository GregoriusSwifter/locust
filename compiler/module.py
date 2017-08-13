#!/usr/bin/env python
# -*- coding: utf-8 -*-
from os import path
from collections import OrderedDict
from string import Template

from .types_ import *
from .ast.collapser import as_c

module_files = OrderedDict()

class Package(object):
	def __init__(self, modules=[], name="main"):
		self.name = name
		self.modules = OrderedDict()
		for module in modules:
			self.add_module(module)
	
	def add_module(self, filename):
		global module_files
		module.package = self
		self.modules[module.filename] = module

class Module:
	def __init__(self, file_, filename):
		global module_files
		
		# Checking to see if we already exist as a module
		if filename in module_files:
			m = module_files[filename]
			self = m
		else:
			module_files[filename] = self
		
		self.imports = OrderedDict()
		self.scope = OrderedDict()
		self.nodes = OrderedDict()
		self.file_ = file_
		self.name = filename
	
	def do_import(self, file, name):
		self.imports[name] = Module(file, name)
		
	def __str__(self):
		t = Template("package $package\n\n$code\n")
		return t.substitute(package=self.name, code='\n'.join([as_c(node) for node in self.nodes]))
