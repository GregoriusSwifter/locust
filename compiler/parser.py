#!/usr/bin/env python
# -*- coding: utf-8 -*-
import inspect
import sys
from collections import deque, OrderedDict

from .ast import nodes
from .lexer import *
from .settings import *
import compiler.types_ as Types


class ParseError(Exception):
    pass

class TokensView(object):
    def __init__(self, lexer):
        self.tokens = lexer.tokens()
        self.behind = deque()
        try:
            self.current = next(self.tokens)
        except StopIteration:
            self.current = None
        try:
            self.next = next(self.tokens)
        except StopIteration:
            self.next = None

    def advance(self, n):
        while n > 0:
            self.behind.append(self.current)
            self.current = self.next
            try:
                self.next = next(self.tokens)
            except StopIteration:
                self.next = None
            n -= 1

# OrderedDict class so we can get the parser methods in the order defined.
class __OrderedClass(type):
     @classmethod
     def __prepare__(metacls, name, bases, **kwds):
        return OrderedDict()

     def __new__(cls, name, bases, classdict):
        result = type.__new__(cls, name, bases, dict(classdict))
        result.members = tuple(classdict)
        return result

class Parser(metaclass=__OrderedClass):
    binop_precedence = {
        '<': 10,
        '+': 20,
        '-': 20,
        '*': 40,
    }

    def __init__(self, module, lexer):
        self.module = module
        self.lexer = lexer
        self.current_owner = None
        self.tokens = TokensView(lexer)

    def advance(self, n=1):
        self.tokens.advance(n)

    def cur_pos(self):
        return self.lexer.cur_line, self.lexer.cur_col

    def cur_pos_str(self):
        return "line: %s, column: %s" % self.cur_pos()

    def get_precedence(self):
        if isinstance(self.tokens.current, SymbolToken):
            return binop_precedence.get(self.tokens.current.value, -1)
        else:
            return -1

    def get_associativity(self):
        return -1

    def error_if_not_exact(self, token):
        if not self.tokens.current == token:
            self.error("Expected token: "+repr(token))

    def error_if_exact(self, token):
        if self.tokens.current == token:
            self.error("Unexpected token: "+repr(token))

    def error_if_not(self, classes):
        if not isinstance(self.tokens.current, classes):
            self.error("Expected token type: "+str(classes))

    def error_if(self, classes):
        if isinstance(self.tokens.current, classes):
            self.error("Unexpected token type: "+str(classes))

    def error(self, text):
        assert isinstance(text, str)
        text += "\n\tGot '%s' at %s" % (repr(self.tokens.current), self.cur_pos())
        raise ParseError(text)

    def nodes(self):
        while True:
            if self.tokens.current == None:
                return
            thing = self.p_top()
            print("!!!"+str(thing))
            yield thing

    @classmethod
    def print_tree(cls):
        print("Locust EBNFish Parse Tree")
        print("Key:")
        print("    'thing'                     Means literally the text of thing, whatever symbol(s) it may be")
        print("    thing                       Lowercase text means another parse rule in the parse tree named thing")
        print("    THING                       Uppercase text is a lexical element named thing")
        print("    ( thing1 thing2 ) thing3    Groups thing1 and thing2 together, but not thing3")
        print("    [ thing ]                   Makes thing optional")
        print("    thing | otherthing          Can be either thing or otherthing")
        print("    thing { otherthing }        Allows any quantity of otherthings, from 0 to infinity")
        print("    thing {+ otherthings +}     Allows any quantity of otherthings, from 1 to infinity")
        print("    %keyword=value              Denotes that the parse rule method takes a keyword argument of this form,")
        print("                                and if it's this value, this line of the rule is authoritative")
        print("")
        print("Tree:")

        def trim(docstring):
            if not docstring:
                return ''
            # Convert tabs to spaces (following the normal Python rules)
            # and split into a list of lines:
            lines = docstring.expandtabs().splitlines()
            # Determine minimum indentation (first line doesn't count):
            indent = sys.maxsize
            for line in lines[1:]:
                stripped = line.lstrip()
                if stripped:
                    indent = min(indent, len(line) - len(stripped))
            # Remove indentation (first line is special):
            trimmed = [lines[0].strip()]
            if indent < sys.maxsize:
                for line in lines[1:]:
                    trimmed.append(line[indent:].rstrip())
            # Strip off trailing and leading blank lines:
            while trimmed and not trimmed[-1]:
                trimmed.pop()
            while trimmed and not trimmed[0]:
                trimmed.pop(0)
            # Return a single string:
            return '\n'.join(trimmed)

        docstrings = [trim(cls.__dict__[method].__doc__) for method in cls.members
                          if method.startswith('p_')
                          and callable(cls.__dict__[method])
                          and cls.__dict__[method].__doc__ is not None]
        prefix_lens = [docstring.find("=") for docstring in docstrings]
        prefix_lens_max = max(prefix_lens)

        for i, docstring in enumerate(docstrings):
            prefix_len = prefix_lens[i]
            s = ' '*(prefix_lens_max-prefix_len)
            print(s+docstring.replace('\n', '\n'+s).rstrip())
        print("")

    def p_top(self):
        """
        top = import
            | if
            | for
            | func_def
            | obj_def
            | iface_def
            | assignment
            | call
            | expression
        """
        token = self.tokens.current
        if isinstance(token, KeywordToken):
            if token == KeywordToken('import'):
                return self.p_import()
            if token == KeywordToken('if'):
                return self.p_if()
            if token == KeywordToken('for'):
                return self.p_for()
            if token == KeywordToken('while'):
                return self.p_while_loop()
            if token == KeywordToken('func'):
                return self.p_func_def()
            if token == KeywordToken('obj'):
                return self.p_obj_def()
            if token == KeywordToken('iface'):
                return self.p_iface_def()
        elif isinstance(token, IDToken):
            if self.next == SymbolToken('('):
                return self.p_call()
            else:
                return self.p_assignment()
        return self.p_expression()

    def p_mid(self):
        """
        mid = if
            | for
            | assignment
            | call
            | expression
        """
        token = self.tokens.current
        if isinstance(token, KeywordToken):
            if token == KeywordToken('import'):
                self.error("Got 'import' in disallowed context. (function, object, etc.)")
            if token == KeywordToken('if'):
                return self.p_if()
            if token == KeywordToken('for'):
                return self.p_for()
            if token == KeywordToken('while'):
                return self.p_while_loop()
            if token == KeywordToken('func'):
                self.error("Got 'func' in disallowed context. (another function, etc.)")
        elif isinstance(token, IDToken):
            if self.next == SymbolToken('('):
                return self.p_call()
            else:
                return self.p_assignment()
        return self.p_expression()

    def p_import(self):
        """
        import = 'import' namespaced [('as' IDENTIFIER | '.*')]
        """
        self.error_if_not_exact(KeywordToken('import'))
        self.advance()
        import_path = self.p_namespaced()
        while True:
            if self.tokens.current == SymbolToken('.'):
                self.advance()
                if self.tokens.current == IDToken():
                    import_path.append(self.tokens.current)
                    self.advance()
                elif self.tokens.current == SymbolToken('*'):
                    self.advance()
                    return nodes.Import(self.module, import_path, True)
                else:
                    self.error("Expected either an identifier or * wildcard after . in import")
            elif self.tokens.current == KeywordToken('as'):
                self.advance()
                self.error_if_not(IDToken)
                return nodes.Import(self.module, import_path, alias=self.tokens.current)
            else:
                return nodes.Import(self.module, import_path)

    def p_if(self):
        """
        if = 'if' boolean EOL INDENT {top} DEDENT
              {'else' 'if' boolean EOL INDENT {top} DEDENT}
              ['else' EOL INDENT {top} DEDENT]
        """
        self.current_indent = self.lexer.cur_indent

        conditions = []
        branches = [[],]

        # Initial if statement
        self.error_if_not_exact(KeywordToken('if'))
        self.advance()
        conditions.append(self.p_conditional())
        self.error_if_not(EOLToken)
        self.advance()
        self.error_if_not(IndentToken)
        self.advance()
        while True:
            if self.tokens.current == DedentToken():
                self.advance()
                break
            elif self.tokens.current == EOFToken():
                break
            branches[-1].append(self.p_top())

        # Optional else ifs and else
        while True:
            if self.tokens.current != KeywordToken('else'):
                break
            self.advance()
            # Else If
            if self.tokens.current == KeywordToken('if'):
                self.advance()
                conditions.append(self.p_conditional())
                self.error_if_not(EOLToken)
                self.advance()

                branches.append([])
                while True:
                    if self.tokens.current == DedentToken():
                        self.advance()
                        break
                    elif self.tokens.current == EOFToken():
                        break
                    branches[-1].append(self.p_top())
            # Else
            elif isinstance(self.tokens.current, EOLToken):
                self.advance()
                conditions.append(None)
                self.error_if_not(IndentToken)
                self.advance()

                branches.append([])
                while True:
                    if self.tokens.current == DedentToken():
                        self.advance()
                        break
                    elif self.tokens.current == EOFToken():
                        break
                    branches[-1].append(self.p_top())
            # What?!
            else:
                self.error("Expected either 'if' or EOL after 'else'")
        return nodes.ConditionalBranching(self.module, conditions, branches)

    def p_boolean(self):
        """
        boolean = expression { ['>' | '<' | '=' | '!=' | '>=' | '<=' | 'and' | 'or'] boolean }
        """
        eval_queue = []
        eval_queue.append(self.p_boolean())

    def p_switch(self):
        """
        switch = 'switch' [ IDENTIFIER ] EOL
                 {+ INDENT switch_case EOL INDENT {top} DEDENT +}
        """

    def p_switch_case(self):
        """
        switch_case = 'on'
        """

    def p_for(self):
        """
        for = 'for'                                       EOL INDENT {top} DEDENT
            | 'for' boolean                               EOL INDENT {top} DEDENT
            | 'for' IDENTIFIER 'in' (terminal | interval) EOL INDENT {top} DEDENT
        """

        self.current_indent = self.lexer.cur_indent
        body = []
        conditions = None
        iteration = None

        self.error_if_not_exact(KeywordToken('for'))
        self.advance()

        if isinstance(self.tokens.current, EOLToken):
            # Forever loop
            pass
        elif isinstance(self.tokens.current, IDToken):

            # Iteration
            iteration = 'derp'
            identifier = self.p_namespaced()

            self.advance()
            self.error_if_not_exact(KeywordToken('in'))
            self.advance()

        else:
            # Boolean or error
            conditions = self.p_boolean()

        self.error_if_not(EOLToken)
        self.advance()
        self.error_if_not(IndentToken)
        self.advance()
        while True:
            if self.tokens.current == DedentToken():
                self.advance()
                break
            elif self.tokens.current == EOFToken():
                break
            body.append(self.p_top())

        # build AST
        if iteration is not None:
            return nodes.Iteration(self.module, 'derp', body)
        else:
            return nodes.Loop(self.module, conditions, body)

    def p_var_decl(self):
        """
        var_decl = [type ] IDENTIFIER
        """
        print("var_decl! %s" %(repr(self.tokens.current)))
        type = None
        if isinstance(self.tokens.current, KeywordToken):
            try:
                print(dir(Types))
                type = getattr(Types, str(self.tokens.current))()
            except IndexError:
                pass

            if type is not None:
                self.advance()
            else:
                # keyword used, but isn't a type, so keyword used where identifier expected, most likely.
                self.error("Keyword found where identifier expected, or optionally a type then identifier.")

        self.error_if_not(IDToken)
        id = self.tokens.current
        self.advance()
        if type is None:
            type = "var"
        return nodes.Variable(self.module, id, type)

    def p_func_proto(self):
        """
        func_proto = 'func' IDENTIFIER
                     '(' {var_decl ','} [var_decl] ')'
                     [ {var_decl ','} [var_decl] ]
                     EOL
        """
        self.error_if_not_exact(KeywordToken('func'))
        self.advance()
        self.error_if_not(IDToken)
        id = self.tokens.current
        self.advance()
        self.error_if_not_exact(SymbolToken('('))
        self.advance()

        # Parse the argument list
        args = []
        if self.tokens.current == SymbolToken(')'):
            self.advance()
        else:
            while True:
                args.append(self.p_var_decl())
                if self.tokens.current == SymbolToken(')'):
                    self.advance()
                    break;
                elif self.tokens.current == SymbolToken(','):
                    self.advance()
                    # trailing commas are okay, goddammit
                    if self.tokens.current == SymbolToken(')'):
                        self.advance()
                        break;
                    continue
                else:
                    self.error("Expected either a ',' or ')' to continue/close argument list")


        ret_args = []
        # Parse optionally the return argument list
        while isinstance(self.tokens.current, KeywordToken) or isinstance(self.tokens.current, IDToken):
            ret_args.append(self.p_var_decl())
            if self.tokens.current == SymbolToken(','):
                self.advance()
                # trailing commas are okay, goddammit
            else:
                # Anything other than a var decl or , is unexpected
                self.error_if_not(EOLToken)
        self.p_eols()
        return nodes.FunctionProto(self.module, id, returns=ret_args, owner=self.current_owner, args=args)

    def p_func_def(self):
        """
        func_def = func_proto
                   INDENT
                   mid {mid}
                   DEDENT
        """
        proto = self.p_func_proto()
        self.error_if_not(IndentToken)
        body = []
        while self.tokens.current != DedentToken():
            body.append(self.p_mid())
        return nodes.FunctionDef(self.module, proto, body)

    def p_obj_def(self):
        """
        obj_def = 'obj' IDENTIFIER EOL
                  INDENT
                  {var_decl EOL}
                  {func_def}
                  DEDENT
        """
        self.error_if_not_exact(KeywordToken('obj'))
        self.advance()
        self.error_if_not(IDToken)
        id = self.tokens.current
        self.advance()
        self.p_eols()
        self.error_if_not(IndentTokent)

        obj = nodes.Object(self.module, id)

        while True:
            if self.tokens.current == KeywordToken('func'):
                while self.tokens.current != DedentToken():
                    func = self.p_func_def()
                    obj.add_function(func)

            var = self.p_var_decl()
            obj.add_field(var)

            self.error_if_not(EOLToken)
            self.advance()

            if self.tokens.current == DedentToken():
                break
        return obj

    def p_iface_def(self):
        """
        iface_def = 'iface' IDENTIFIER EOL
                    INDENT
                    {var_decl EOL}
                    {func_proto}
                    DEDENT
        """
        self.error_if_not_exact(KeywordToken('iface'))
        self.advance()
        self.error_if_not(IDToken)
        id = self.tokens.current
        self.advance()
        self.p_eols()
        self.error_if_not(IndentToken)
        self.advance()
        iface = nodes.Iface(self.module, id)

        while True:
            if self.tokens.current == KeywordToken('func'):
                # once there's a func in iface, vars should not happen
                while self.tokens.current not in (None, DedentToken()):
                    func = self.p_func_proto()
                    iface.add_func(func)
                return iface

            var = self.p_var_decl()
            print("#$ got var: "+repr(var))
            iface.add_var(var)

            self.p_eols()
            if self.tokens.current == DedentToken():
                self.advance()
                break
        return iface

    def p_expression(self):
        """
        expression = (parenthesized | number | namespaced | call) [op expression]
                   | '-' expression %RIGHT
                   | '+' expression %RIGHT
                op = ('*' | '/' | '+' | '-')
        """
        token = self.tokens.current

        if token == SymbolToken('('):
            left = self.p_parenthesized()
        elif isinstance(token, NumberToken):
            left = self.p_number()
        elif isinstance(token, IDToken):
            if self.next == SymbolToken('('):
                left = self.p_call()
            else:
                left = self.p_namespaced()
        elif token == SymbolToken('-'):
            self.advance()
            return nodes.UnaryOp(self.module, '-', self.p_expression())
        elif token == SymbolToken('+'):
            self.advance()
            return nodes.UnaryOp(self.module, '+', self.p_expression())
        else:
            self.error("Unexpected token in expression")
        if token not in (SymbolToken('*'),
                         SymbolToken('/'),
                         SymbolToken('+'),
                         SymbolToken('-')):
            return left
        else:
            return self.p_binop_rhs(left, 0)

    def p_binop_rhs(self, left, left_precedence):
        while True:
            precedence = self.get_precedence()
            if precedence < left_precedence:
                return left
            binary_op = self.tokens.current
            self.next()
            right = self.p_expression()

            next_precedence = self.get_precedence()
            if precedence < next_precedence:
                right = self.p_binop_rhs(right, precedence+1)
            left = BinaryOp(self.module, binary_operator, left, right)

    def p_parenthesized(self):
        """
        parenthesized = '(' expression ')'
        """
        self.error_if_not_exact(SymbolToken('('))
        self.advance()
        sub = self.p_expression()
        self.error_if_not_exact(')')
        self.advance()
        return sub

    def p_literal(self):
        """
        literal = number | string
        """
        token = self.tokens.current
        if isinstance(token, NumberToken):
            return self.p_number()
        self.error_if_not(StringToken)

    def p_terminal(self):
        """
        terminal = call | namespaced | literal
        """
        token = self.tokens.current
        if isinstance(token, IDToken):
            if self.next == SymbolToken('('):
                return self.p_call()
            else:
                return self.p_namespaced()
        elif isinstance(token, NumberToken):
            return self.p_number()
        elif isinstance(token, StringToken):
            return nodes.String(self.module, token.value)
        else:
            self.error('Expected terminal, got %s' % token)

    def p_call(self):
        """
        call = namespaced '(' [expression {',' expression}] ')'
        """
        name = self.p_namespaced()
        self.advance()
        self.error_if_not_exact(SymbolToken('('))
        self.advance()
        args = []
        while True:
            if self.tokens.current == SymbolToken(')'):
                break
            else:
                args.append(self.p_expression())
                self.error_if_not_exact(SymbolToken(','))
        return nodes.FunctionCall(self.module, identifier, args)

    def p_number(self):
        """
        number = FLOAT | INTEGER
        """
        token = self.tokens.current
        if isinstance(token, IntToken):
            type = 'int'
        elif isinstance(token, FloatToken):
            type = 'float'
        else:
            raise RuntimeError('Expected number, got %s' % token)
        result = nodes.Number(self.module, type, token.value)
        self.advance()
        return result

    def p_namespaced(self):
        """
        namespaced = IDENTIFIER {'.' IDENTIFIER}
        """
        id_chain = []
        self.error_if_not(IDToken)
        id_chain.append(self.tokens.current)
        self.advance()
        while True:
            if self.tokens.current != SymbolToken('.'):
                break
            self.advance()
            self.error_if_not(IDToken)
            id_chain.append(self.tokens.current)
            self.advance()
        ident = '.'.join(map(str, id_chain))
        return ident

    def p_assignment(self):
        """
        assignment = namespaced ['as' type] ('=' | '+=' | '-=' | '*=' | '/=') expression EOL
        """
        to = self.p_namespaced()
        cast = None
        if self.tokens.current == KeywordToken('as'):
            self.advance()
            cast = self.p_type()
        if self.tokens.current == SymbolToken('='):
            self.advance()
            value = self.p_expression()
            self.p_eols()
            return nodes.Assignment(self.module, to, cast, value)
        else:
            if self.tokens.current == SymbolToken('+'):
                self.advance()
                self.error_if_not_exact(SymbolToken('='))
                self.advance()
                op = '+'
            elif self.tokens.current == SymbolToken('-'):
                self.advance()
                self.error_if_not_exact(SymbolToken('='))
                self.advance()
                op = '-'
            elif self.tokens.current == SymbolToken('*'):
                self.advance()
                self.error_if_not_exact(SymbolToken('='))
                self.advance()
                op = '*'
            elif self.tokens.current == SymbolToken('/'):
                self.advance()
                self.error_if_not_exact(SymbolToken('='))
                self.advance()
                op = '/'
            else:
                self.error("Expected one of: = += -= *= or /= for assignment.")
            value = self.p_expression()
            self.p_eols()
            return nodes.Alteration(self.module, to, op, value)

    def p_interval(self):
        """
        interval = terminal 'to' terminal {'by' terminal}
        """
        start = self.p_terminal()
        self.error_if_not_exact(KeywordToken('to'))
        self.advance()
        end = self.p_terminal()
        if self.tokens.current == KeywordToken('by'):
            self.advance()
            by = self.p_terminal()
            return nodes.Range(self.module, start, end, step)
        else:
            return nodes.Range(self.module, start, end)

    def p_sub_interval(self):
        """
        sub_interval = (namespaced | call) '[' [interval] ']'
        """
        name = self.p_namespaced()
        self.advance()
        if self.tokens.current == SymbolToken('('):
            pass

        args = []
        while True:
            if self.tokens.current == SymbolToken(')'):
                break
            else:
                args.append(self.p_expression())
                self.error_if_not_exact(SymbolToken(','))
        return nodes.FunctionCall(self.module, identifier, args)

    def p_type_primitive(self, optional=False):
        """
        type_primitive = %optional=True [
                            'int8' |  'int16' |  'int32'  |  'int64'
                         | 'uint8' | 'uint16' | 'uint32'  | 'uint64'
                         |  'int'  | 'float'  | 'float32' | 'float64' |   'bool'
                         |  'byte' | 'error'  |   'ptr'   |   'func'  |   'iface'
                         %optional=True ]
        """
        if not isinstance(self.tokens.current, KeywordToken) and not optional:
            self.error("Expected a primitive type name")
        try:
            type = Types.primitives[str(self.tokens.current)]
            self.advance()
            return type
        except KeyError:
            if optional:
                return None
        self.error("Expected a primitive type name")

    def p_type_compound(self, optional=False):
        """
        type_compound = %optional=True [
                         'array' | 'vector' | 'deque' | 'llist' | 'dllist'
                        | 'map'  |  'chan'  |  'ring' |  'set'  |  'bag'
                        %optional=True ]
        """
        if not isinstance(self.tokens.current, KeywordToken) and not optional:
            self.error("Expected a compound type name")
        try:
            type = Types.compounds[str(self.tokens.current)]
            self.advance()
            return type
        except KeyError:
            if optional:
                return None
        self.error("Expected a compound type name")

    def p_type(self, optional=False):
        """
        type = %optional=True [
               (type_primitive | type_compound) {type_compound}
               %optional=True ]
        """
        if not isinstance(self.tokens.current, KeywordToken):
            if optional:
                return None
            self.error("Expected a type keyword")

        prim = self.p_type_primitive()
        if prim is None:
            self.error("Expected a type")
        comp = self.p_type_compound()
        while comp is not None:
            comp = self.p_type_compound()
        print("Type: "+str(type))
        return type

    def p_eols(self, optional=False):
        """
        eols = %optional=False EOL
               {EOL}
        """
        if not optional:
            self.error_if_not(EOLToken)
            self.advance()
        while isinstance(self.tokens.current, EOLToken):
            print("checking for eols: "+repr(self.tokens.current))
            self.advance()
        return

if __name__ == '__main__':
    """
    Invoke with something like:
        E:\projects\locust>python3 -m compiler.parser compiler\parser.py
    in order to get parse tree output.
    """
    Parser.print_tree()
