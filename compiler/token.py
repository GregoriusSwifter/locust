#!/usr/bin/env python

class Token(object):
    def __init__(self, value=None, start=None, end=None, associativity=None, precedence=None):
        self.value = value
        self.start = start
        self.end = end
        self.associativity = associativity
        self.precedence = precedence
        
    def __str__(self):
        return str(self.value)
        
    def __repr__(self):
        s = '<%s' % self.__class__.__name__
        if self.value is not None:
            s += ' value:%s' % str(self.value)
        s += '>'
        return s

    def __eq__(self, other):
        try:
            if not isinstance(other, self.__class__) or self.value != other.value:
                return False
            else:
                return True
        except NameError:
            return False
    
    def __ne__(self, other):
        try:
            if not isinstance(other, self.__class__) or self.value != other.value:
                return True
            else:
                return False
        except NameError:
            return True

    def __lt__(self, other):
        if not isinstance(other, self.__class__) or self.value >= other.value:
            return False
        return True

    def __le__(self, other):
        if not isinstance(other, self.__class__) or self.value > other.value:
            return False
        return True

    def __gt__(self, other):
        if not isinstance(other, self.__class__) or self.value <= other.value:
            return False
        return True

    def __ge__(self, other):
        if not isinstance(other, self.__class__) or self.value < other.value:
            return False
        return True

class KeywordToken(Token):
    pass

class SymbolToken(Token):
    pass

class StringToken(Token):
    pass

class IndentToken(Token):
    pass
    
class DedentToken(Token):
    pass

class SpaceToken(Token):
    pass

class TerminalToken(Token):
    pass

class IDToken(TerminalToken):
    pass

class NumberToken(TerminalToken):
    pass

class FloatToken(NumberToken):
    pass

class IntToken(NumberToken):
    pass

class EOLToken(Token):
    pass
    

