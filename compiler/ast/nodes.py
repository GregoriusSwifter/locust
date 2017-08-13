#!/usr/bin/env python

from string import Template
from collections import OrderedDict

import types as Types


def handle_type_coercion(left, right):
    # TODO: Implement compile-time type checking when necessary.
    return (left, right)

def handle_type_operator(left_type, right_type, operator):
    if operator == '+':
        return 'add', 'add_tmp'
        return getattr(builder, op)(left, right, op_name)
    elif operator == '-':
        return getattr(builder, op)(left, right, op_name)
    elif operator == '*':
        return getattr(builder, op)(left, right, op_name)
    elif operator == '/':
        pass
    elif operator == '%':
        pass
    elif operator == '^':
        pass
    elif operator == '>':
        pass
    elif operator == '<':
        result = g_llvm_builder.fcmp(FCMP_ULT, left, right, 'cmptmp')
        return self.module.llvm.uitofp(result, 'booltmp')
    RuntimeError('Unknown binary operator.')

class Base(object):
    def __init__(self, module):
        self.module = module
        
    def as_c(self):
        raise NotImplementedError("AST Node has no as_c() method.")

class Number(Base):
    def __init__(self, module, type, value):
        Base.__init__(self, module)
        self.type = type
        self.value = value
        
    def __str__(self):
        return str(self.value)
    
    def as_c(self):
        return str(self)

class String(Base):
    def __init__(self, module, value):
        Base.__init__(self, module)
        self.value = value
    
    def __str__(self):
        return self.value
    
    def as_c(self):
        return "\""+self.value+"\""

class Variable(Base):
    c_init_t = Template("= ${value}");
    c_decl_t = Template("${type} ${name}${init};\n")

    def __init__(self, module, id, type, value=None):
        Base.__init__(self, module)
        self.type = type
        self.name = id
        self.value = value
        
    def as_c(self):
        t = dict()
        t["type"] = str(self.type)
        t["name"] = str(self.name)
        t["init"] =  ''
        if self.value is not None:
            t["init"] = c_init_t.substitute(value=str(self.value))
        
        return c_decl_t.substitute(**t)
    
    def __str__(self):
        return str(self.name)


class Object(Base):
    def_t = Template("${struct}${type_id}${fields_meta}${methods_meta}${struct_meta}")
    struct_t = Template("${pack_before}\ntypedef struct {\n${fields}\n} ${name};\n${pack_after}\n")
    type_id_t = Template("__type_id ${name}__type_id = ${type_id};\n")
    
    fields_meta_t = Template("""
        static __fields_list ${name}__meta_fields = {
            ${fields_count}, {
                ${fields_infos}
            }
        };
    """)
    field_info_t = Template("{${type}__type_id, sizeof(${type}), (byteptr)\"${name}\")},\n")
    
    methods_meta_t = Template("""
        static __methods_list ${name}__meta_methods= = {
            ${methods_count}, {
                ${method_infos}
            }  
        };
    """)
    method_info_t = Template("""{${type}__type_id, ${arg_count}, (byteptr)\"${name}\", ${method_ptr}, {
        ${method_arg_types}
    }},
    """)
    
    struct_meta_t = Template("""
        static meta ${name}__meta = {
            ${name}__type_id,
            sizeof(${name}),
            (byteptr)"${name}",
            ${fields_meta_ptr},
            ${methods_meta_ptr},
        };
    """)
    
    def __init__(self, module, id):
        Base.__init__(self, module)
        self.name = id
        self.fields = OrderedDict()
        self.funcs = OrderedDict()

    def __str__(self):
        return str(self.name)
    
    def add_field(self, variable):
        # Nameless structs have their fields overwritten onto ours
        if variable.name is None and isinstance(variable.type, Struct):
            for k,v in variable.type.fields.items():
                self.fields[k] = v
        else:
            self.fields[name] = v
            
    def add_function(self, function):
        function.prototype.owner = self
        self.funcs[function.name] = function
    
    def as_c(self):
        # form meta info for fields
        fields_count = len(self.fields)
        fields = list()
        for k,v in self.fields.items():
            fields.append(field_info_t.substitute(type=v, name=k))
        fields_infos = ",\n".join(fields)
        fields_meta = fields_meta_t.substitute(name=self.name,
                                               fields_count=fields_count,
                                               fields_infos=fields_infos)
        
        # form meta info for funcs
        funcs_count = len(self.funcs)
        funcs = list()
#        for k,v in self.funcs.items():
#            funcs.append(
#        funcs_defs = ",\n".join(["{%s__type_id, }"])
        # & META_FIELDS_OF(meta),

class Iface(Base):
    field_info_t = Template("{${type}__type_id, sizeof(${type}), (byteptr)\"${name}\")},\n")
    fields_list_t = Template("""
        static fields_list ${name}__fields_list = {
            ${fields_count}, {
                ${fields_infos}
            }
        };
    """)
    
    takes_list_t = Template("""
	static args_list ${name}__${method_name}_takes_args_list = {${takes_len}, ${takes_type_ids}};		   
    """)
    
    rets_list_t = Template("""
        static args_list ${name}__${method_name}_rets_args_list = {${rets_len}, ${rets_type_ids}};
    """)
    
    method_info_t = Template("""{ .takes = & ${name}__${method_name}_takes_args_list, .returns = & ${name}__${method_name}_rets_args_list, .name = (byteptr)\"${name}\" };""")
    methods_list_t = Template("""
        static methods_list ${name}__methods_list= = {
            ${methods_count}, {
                ${methods_infos}
            }  
        };
    """)
    
    iface_def_t = Template("""
        static iface_def ${name} = {
            ${fields_list_ptr},
            ${methods_list_ptr},
	    (byteptr)"${name}",
        };
    """)
    
    def __init__(self, module, id):
        Base.__init__(self, module)
        self.name = id
        self.vars = OrderedDict()
        self.funcs = OrderedDict()

    def __str__(self):
        return str(self.name)
    
    def add_var(self, var):
        # Nameless structs have their fields overwritten onto ours
        if var.name is None and isinstance(variable.type, Struct):
            for k,v in variable.type.fields.items():
                self.vars[k] = v
        else:
            print(repr(var.name))
            self.vars[var.name.value] = var
            
    def add_func(self, func):
        self.funcs[func.name.value] = func
    
    def as_c(self):
        # form meta info for fields
        fields_count = len(self.vars)
        fields = list()
        for k,v in self.vars.items():
            fields.append(self.field_info_t.substitute(type=v, name=k))
        fields_infos = self.fields_list_t.substitute(name=self.name,
                                                fields_count=fields_count,
                                                fields_infos=",\n".join(fields))
        
        # form meta info for funcs
        methods_count = len(self.funcs)
        prereqs = list()
        funcs = list()
        for k,func in self.funcs.items():
            rets, rets_len = ",\n".join(["META_OF("+str(arg.type)+")" for arg in func.args]), len(func.args)
            takes, takes_len = ",\n".join(["META_OF("+str(arg.type)+")" for arg in func.attrs]), len(func.attrs)
            prereqs.append(self.takes_list_t.substitute(name=self.name,
                                                   method_name=func.name,
                                                   takes_len=takes_len,
                                                   takes_type_ids=takes))
            prereqs.append(self.rets_list_t.substitute(name=self.name,
                                                  method_name=func.name,
                                                  rets_len=rets_len,
                                                  rets_type_ids=rets))
            funcs.append(self.method_info_t.substitute(name=self.name,
                                                  method_name=func.name))
        methods_infos = self.methods_list_t.substitute(name=self.name,
                                                  methods_count=methods_count,
                                                  methods_infos=",\n".join(funcs))

        iface_def = self.iface_def_t.substitute(name=self.name,
                                         fields_list_ptr="& "+str(self.name)+"__fields_list",
                                         methods_list_ptr="& "+str(self.name)+"__methods_list")
        return "\n\n".join(prereqs)+"\n"+fields_infos+"\n"+methods_infos+"\n"+iface_def+"\n"

class Assignment(Base):
    c_t = Template("${to} = ${cast}${value};\n")
    
    def __init__(self, module, to, value, cast=None):
        Base.__init__(self, module)
        self.to = to
        self.value = value
        self.cast = cast
        
    def as_c(self):
        t = dict()
        t["to"] = str(self.to)
        t["value"] = str(self.value)
        t["cast"] = ''
        if self.cast is not None:
            t["cast"] = '('+str(self.cast)+')'
            
        return t.substitute(**t)


class Alteration(Base):
    t = Template("${to} ${op}= ${value}")
    
    def __init__(self, module, to, op, value):
        Base.__init__(self, module)
        self.to = to
        self.op = op
        self.value = value
        
    def as_c(self):
        return t.substitute(to=str(self.to), op=str(self.op), value=str(self.value))


class UnaryOp(Base):
    t = Template("${op} ${value}")
    
    def __init__(self, module, operater, value):
        Base.__init__(self, module)
        self.operator = operater
        self.value = value
        
    def as_c(self):
        return t.substitute(op=str(self.operator), value=str(self.value))


class BinaryOp(Base):
    t = Template("${left} ${op} ${right}")
    
    def __init__(self, module, operater, left, right):
        Base.__init__(self, module)
        self.operator = operater
        self.left = left
        self.right = right
        
    def as_c(self):
        return (self.left.as_c, str(self.operator), self.right.as_c)

class FunctionProto(Base):
    c_decl_t = Template("${attrs} ${returns} ${owner}${name} (${args})${end}")

    def __init__(self, module, name, owner=None, returns=[], args=[], attrs=[]):
        Base.__init__(self, module)
        self.name = name
        self.owner = owner
        self.returns = returns
        self.args = args
        self.attrs = attrs
        self.params_len = len(returns) + len(args)

    def build_c_prototype(self, is_decl=True):
        t = dict()
        t["name"] = self.name
        t["args"] = ', '.join([str(arg) for arg in self.args])
        t["returns"] = ', '.join([str(ret) for ret in self.returns])
        t["attrs"] = ' '.join([str(attr) for attr in self.attrs])
        
        t["owner"] = ''
        if self.owner is not None:
            t["owner"] = str(self.owner)+'__m_'
        t["end"] = ';\n' if is_decl else ''
        
        return c_decl_t.substitute(**t)

    def as_c(self):
        return self.build_c_prototype(is_decl=True)

class FunctionDef(Base):
    c_def_front_t = Template("${prototype} {")
    c_def_back_s = "\n}\n"
    
    def __init__(self, module, prototype, body):
        Base.__init__(self, module)
        self.prototype = prototype
        self.body = body
        self.stack_size = 0
        self.has_defers = False
        self.has_errors = False
    
    def as_asm(self):
        t = {
            "name": self.prototype.name,
            "stack_size": self.stack_size,
            "defers": "",
            "errors": "",
        }
        if self.has_defers:
            t["defers"] = "CALL rt.defers"
        if self.has_errors:
            t["errors"] = "CALL rt.errors"
        asm_def = asm_def_t.substitute(**t)
        
                                    
    def as_c(self):
        proto = self.prototype.build_c_prototype(is_decl=False)
        c_def_front = c_def_front_t.substitute(prototype=proto)
        self.body.insert(0, c_def_front)
        self.body.append(c_def_back_s)
        return '\n\t'.join(self.body)
    

class FunctionCall(Base):
    call_t = Template("""subq \$${args_bytes}, %rsp;
        ${arg_assignments}
        ${extra}
    """)
    arg_assign_t = Template("MOV${size_postfix} ${value}, ${offset}(%rsp)")

    def __init__(self, module, function, args=[], go=False, defer=False):
        Base.__init__(self, module)
        self.func = function
        self.args = args
        self.is_go = go
        self.is_defer = defer
        if len(self.args) != len(self.func.args):
            raise RuntimeError('Incorrect number of arguments passed.')
        
    def as_c(self):
        t = dict()
#        t["name"] = 
        arg_values = [arg.to_c() for arg in self.args]
        args = ', '.join(arg_values)
        args.insert(0, str(self.func.name)+"(")
        args.append(")")
        return c_call_t.substitute(**t)


class Coroutine(Base):
    c_t = Template("""typedef struct { bool done; ${returns} } ${name}_ret;
					typedef ${name}_ret(^${name}_block)(${block_args});
					${name}_block ${name}(${initial_args}) {}""")
    # TODO: Finish template
    def __init__(self, module, name, returns, initial_args, block_args, body):
        Base.__init__(self, module)
        self.name = name
        self.returns = returns
        self.prototype = prototype
        self.body = body

    def as_c(self):
        t = dict()
        t["returns"] = ''
        return c_t.substitute(**t)


class ConditionalBranching(Base):
    c_first_t = Template("if (${cond}) {\n\t${branch}\n}\n")
    c_elseif_t = Template("else if (${cond}) {\n\t${branch}\n}\n")
    c_else_t = Template("else {\n\t${branch}\n}\n")
    
    def __init__(self, module):
        Base.__init__(self, module)
        self.conditions = list()
        self.branches = list()
    
    def add_branch(self, condition, branch):
        self.conditions.append(condition)
        self.branches.append(branch)
        
    def has_no_branches(self):
        return len(self.conditions) < 1
        
    def has_multiple_branches(self):
        return len(self.conditions) > 1
        
    def has_else_branch(self):
        return self.has_multiple_branches() and self.conditions[-1] is None
        
    def has_elseif_branches(self):
        penalty = 1 if self.has_else_branch() else 0
        return (len(self.conditions) - penalty) > 1

    def as_c(self):
        if self.has_no_branches():
            return ''
            
        t = dict()
        t["cond"] = str(self.conditions[0])
        t["branch"] = str(self.branches[0])
        ret = c_first_t.substitute(**t)
        
        for cond, branch in zip(self.conditions[1:-1], self.branches[1:-1]):
            t = dict()
            t["cond"] = str(cond)
            t["branch"] = str(branch)
            ret += c_elseif_t.substitute(**t)
            
        if self.has_multiple_branches():
            cond = self.conditions[-1]
            branch = self.branches[-1]
            t = dict()
            t["branch"] = str(branch)
            
            if self.has_else_branch():
                ret += c_else_t.substitute(**t)
            else:
                t["cond"] = str(cond)
                ret += c_elseif_t.substitute(**t)
                
        return ret


class Range(Base):
    def __init__(self, module, start, end, step=1):
        Base.__init__(self, module)
        self.start = start
        self.end = end
        self.step = step
        
    def as_c(self):
        return ''


class Loop(Base):
    def __init__(self, module, conditions=None, body=None):
        Base.__init__(self, module)
        self.conditions = conditions
        self.body = body
    
    def as_c(self):
        return ''


class Import(Base):
    def __init__(self, module, import_path, alias=None, is_wildcard=False):
        Base.__init__(self, module)
        module.do_import() # TODO FIXME
        self.import_path = import_path
        self.is_wildcard = is_wildcard
        
    def as_c(self):
        return ''


class Actor(Base):
    t = Template("""
        func ${func_name}(${func_args}) chan interface{} {
            ret := make(chan interface{})
            ${func_body}
            return ret
        }
    """)
    
    def __init__(self, module, function_definition):
        Base.__init__(self, module)
        self.function = function_definition
    
    def as_c(self):
        # derp
        return ''

class RawC(Base):
    t = Template("""

    """)

    def __init__(self, module, text):
        Base.__init__(self, module)

