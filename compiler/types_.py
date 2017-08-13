#!/usr/bin/env python

from abc import ABCMeta as _ABCMeta
from abc import abstractproperty as _abstractproperty
from collections import OrderedDict as _OrderedDict

class _Type:
    __metaclass__ = _ABCMeta

    @_abstractproperty
    def name(self):
        pass
        
    @_abstractproperty
    def includes(self):
        pass
        
    @_abstractproperty
    def compiles(self):
        pass

    @_abstractproperty
    def size(self): # in bits
        pass

    @_abstractproperty
    def alignment(self): # in bits
        pass

class byte(_Type):
    name = 'byte'
    size = 8
    is_unsigned = True

class nix(_Type):
    name = 'nix'

    def __str__(self):
        return 'nil'
        
def _IntegerTFactory(size=64, is_unsigned=False):
    class IntT(_Type):
        nonlocal size, is_unsigned
        size = size if size in (8, 16, 32, 64, 128) else 64
        is_unsigned = (True and is_unsigned)
        name = 'uint'+str(size) if is_unsigned else 'int'+str(size)
    return IntT
        
def _FloatTFactory(size_=64):
    class FloatT(_Type):
        nonlocal size_
        size = size_ if size_ in (32, 64) else 64
        name = 'float'+str(size)
    return FloatT

class bool(_Type):
    name = 'bool'
        
    def to_go_init(self):
        ret = str(name)
        ret += ' bool :='
        ret += 'true' if self.value else 'false'
        return ret

uint8 = _IntegerTFactory(8, is_unsigned=True)
uint16 = _IntegerTFactory(16, is_unsigned=True)
uint32 = _IntegerTFactory(32, is_unsigned=True)
uint64 = _IntegerTFactory(64, is_unsigned=True)

int8 = _IntegerTFactory(8)
int16 = _IntegerTFactory(16)
int32 = _IntegerTFactory(32)
int64 = _IntegerTFactory(64)

float32 = _FloatTFactory(32)
float64 = _FloatTFactory(64)

#comp32 = _ComplexTFactory(32)
#comp64 = _ComplexTFactory(64)

class obj(_Type, _OrderedDict):
    """Extends OrderedDict to assimilate nameless child objects."""
    name = 'obj'
    includes = []
    compiles = []
    
    def __setitem__(self, key, value):
        if key is None and isinstance(value, struct):
            for k, v in value:
                _OrderedDict.__setitem__(self, k, v)
            return
        _OrderedDict.__setitem__(self, key, value)


class func(_Type):
    name = 'func'
    includes = []
    compiles = []
    
    def __init__(self, name, args, body, return_type=None, is_closure=False):
        self.return_type = return_type if return_type is not None else NixT()
        self.name = name
        self.args = map(str, args)
        self.body = body
        self.is_closure = is_closure
        
    def __repr__(self):
        return 'func'

class actor(_Type):
    name = 'actor'
    includes = ['processes/actors.h']
    compiles = []

class error(_Type):
	name = 'error'
	includes = ['types/error.h']
	compiles = []

class ptr(_Type):
	name = 'ptr'
	includes = ['types/types.h']
	compiles = []

class var(_Type):
	name = 'var'
	includes = ['types/var.h']
	compiles = []

class iface(_Type):
	name = 'iface'
	includes = ['types/iface.h']
	compiles = []

"""
type_primitive ::=  'int8' |  'int16' |  'int32'  |  'int64'  |  'int128'
                 | 'uint8' | 'uint16' | 'uint32'  | 'uint64'  | 'uint128'
                 |  'bool' |  'byte'  | 'float32' | 'float64' |
                 | 'error' |   'str'  |   'ptr'   |   'var'   |  'iface'   
"""
primitives = ( int8,  int16,   int32,   int64,
              uint8, uint16,  uint32,  uint64,
               bool,   byte, float32, float64,
              error,    str,     ptr,     var,   iface)

"""
type_compound ::= 'array' | 'vector' | 'deque' | 'llist' | 'dllist'
                |  'map'  |  'chan'  |  'ring' |  'set'  |  'bag'  
"""
compounds = ()
