#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import string
import codecs
import re

from common.utils import *
from common.token import *

# def X Y enddef
# def X(a, b, c) (Y[a], Y[b], Y[c]) enddef
# undef X
# if defined(X)
# else
# endif

reserved = {
    'if': KeywordToken('if'),
    'else': KeywordToken('else'),
    'nix': KeywordToken('nix'),
    'is': KeywordToken('is'),
    'or': KeywordToken('or', associativity='left', precedence=0),
    'and': KeywordToken('and', associativity='left', precedence=1),
    'not': KeywordToken('not', associativity='right', precedence=5),
}
reserved_regex_str = str('|'.join(reserved.keys()))
reserved_regex = re.compile(reserved_regex_str)

symbols = {
    '=': SymbolToken('='),
    '(': SymbolToken('('),
    ')': SymbolToken(')'),
    '#': SymbolToken('#'),
    ',': SymbolToken(','),
    '{': SymbolToken('{'),
    '}': SymbolToken('}'),
    '"': SymbolToken('"'),
    '\\': SymbolToken('\\'),
}
symbols_regex_str = '|'.join([str('\\'+symbol) if symbol in ('.^$*+?{}\\[]|()') else str(symbol) for symbol in symbols.keys()])
symbols_regex = re.compile(symbols_regex_str)


class LexComplete(Exception):
    pass

class LexError(Exception):
    pass

class Lexer(object):
    def __init__(self, module, input):
        self.module = module
        self.input = input
        self.handle = self.lex_new_line
        self.to_yield_queue = []
        self.cur_line = 0
        self.cur_col = 0
        self.cur_indent = 0
        
    def lex_new_line(self, f):
        """
        Lexes the beginning of a line, for figuring out indentation levels.
        """
        while True:
            ch = f.peek()
            if ch is None:
                # Handle end of file.
                raise LexComplete()
            elif ch == '\t':
                # Count up leading indentation.
                indent_count = self.lex_indent(f)
                diff = indent_count - self.cur_indent
                if diff == 0:
                    return None
                elif diff > 0:
                    self.to_yield_queue.extend([IndentToken() for i in range(diff-1)])
                    return IndentToken()
                elif diff < 0:
                    self.to_yield_queue.extend([DedentToken() for i in range(abs(diff)-1)])
                    return DedentToken()

            elif ch == ' ':
                # Fail loudly on leading spaces
                i = 0
                f.get()
                ch = f.peek()
                while ch == ' ':
                    i += 1
                    f.get()
                    ch = f.peek()
                raise LexError('Leading spaces are not allowed! '+str(i)+' of them on line '+str(self.cur_line))
            else:
                self.handle = self.lex_normal
                return self.lex_normal(f)

    def lex_normal(self, f):
        """
        Lexes normal things that can occur on any line after indentation.
        """
        while True:
            ch = f.peek()
            if ch is None:
                # Handle end of file.
                raise LexComplete()
            elif ch in '\t ':
                # Ignore non-leading whitespace.
                f.get()
                continue
            elif ch in '\r\n':
                # Handle end of lines
                ch = f.get()
                if ch == '\r':
                    pch = f.peek()
                    if pch == '\n':
                        ch = f.get()
                self.handle = self.lex_new_line
                return EOLToken()
            elif ch in UnicodeCommon.U_DIGITS:
                # Handle numbers
                tok = self.lex_nums(f)
                return tok
            elif ch == '.':
                # Handle fp or not
                ch = f.get()
                if f.peek() in UnicodeCommon.U_DIGITS:
                    f.put(ch)
                    return self.lex_nums(f)
                else:
                    return SymbolToken('.')
            elif ch in UnicodeCommon.U_ALPHAS:
                # Handle characters
                return self.lex_letters(f)
            elif ch == '"':
                # Handle string
                return self.lex_str(f)
            elif ch == '#':
                # Handle comment
                self.lex_comment(f)
            elif ch in symbols:
                # Handle common symbols
                return symbols[f.get()]
            else:
                # Ignore shit we don't know.
                f.get()
                
    def lex_indent(self, f):
        """
        Counts and returns the level of indentation.
        """
        num_indent = 0
        while True:
            ch = f.get()
            if ch is None:
                f.put(ch)
                break
            elif ch == '\t':
                num_indent += 1
            elif ch == ' ':
                continue
            else:
                f.put(ch)
                break
        return num_indent
    
    def lex_nums(self, f):
        """
        Lexes numbers, including floating point.
        """
        num = ''
        is_float = False
        while True:
            ch = f.peek()
            if ch in '0123456789':
                num += f.get()
            elif ch == '.':
                num += f.get()
                is_float = True
            else:
                print("$$$"+num+"$$$"+ch+"$$$"+f.peek())
                if is_float:
                    return FloatToken(value=num)
                else:
                    return IntToken(value=num)
    
    def lex_letters(self, f):
        """
        Lexes things that begin with letters.
        """
        ident = f.get()
        while True:
            ch = f.get()
            if ch in UnicodeCommon.U_ALPHANUMERICS:
                ident += ch
            else:
                f.put(ch)
                break
        try:
            return reserved[ident]
        except KeyError:
            return IDToken(ident)
        
    def lex_str(self, f):
        """
        Lexes and accumulates a string.
        """
        prev_was_escape = False
        str_ = f.get()
        while True:
            ch = f.get()
            if ch is None:
                if str_[-1] != '"':
                    str_.append('"')
                return StringToken(str_)
            if ch == '\\':
                str_ += ch
                prev_was_escape = True
            else:
                if ch == '"':
                    str_ += ch
                    if not prev_was_escape:
                        return StringToken(str_)
                else:
                    str_ += ch
                prev_was_escape = False
                
    def lex_comment(self, f):
        """
        Speeds past a comment to the end of line or file.
        """
        while True:
            ch = f.get()
            if ch is None or ch in '\r\n':
                return
    
    def tokens(self):
        """
        Tokens generator, this is what a parser iterates over to get tokens.
        """
        source = self.input.__enter__()
        while True:
            while len(self.to_yield_queue) > 0:
                to_yield = self.to_yield_queue.pop()
                print("###"+str(to_yield))
                yield to_yield
            try:
                self.to_yield_queue.append(self.handle(source))
            except LexComplete:
                print("Breaking Lexing")
                break
        source.__exit__(None, None, None)

