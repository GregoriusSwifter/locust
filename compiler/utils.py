#!/usr/bin/env python

# generator hacks
def delimit(iterable, before=None, after=None):
    """Delimits all of iterable's values with optional before and after values."""
    if before is not None:
        yield before
    for item in iterable:
        yield item
    if after is not None:
        yield after

def interject(iterable, separator):
    """Interjects separator between iterable values."""
    iterable = iterable.__iter__()
    yield iterable.next()
    while True:
        next = iterable.next()
        yield separator
        yield next

def interject_introspect(iterable, separator_callback):
    """Interjects values between iterable values.
    
    separator_callback will be called every time a separator is 
    expected, with the previous and the next value, as "before" and "after"
    keyword arguments, in that order.
    """
    iterable = iterable.__iter__()
    prev = iterable.next()
    yield prev
    while True:
        next = iterable.next()
        yield separator(before=prev, after=next)
        yield next
        prev = next

# file char buffers
class UnicodeCommon:
    U_ALPHAS = '_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
    U_DIGITS = '0123456789'
    U_ALPHANUMERICS = U_ALPHAS+U_DIGITS

class FileCharBuffer(object):
    def __init__(self, file):
        self.file = file
        self.buffer = ''
        
    def __enter__(self):
        """
        Start of a lexing context.
        """
        if isinstance(self.file, str):
            self.f = codecs.open(self.file, encoding='utf-8')
        else:
            self.f = self.file
        return self
    
    def __exit__(self, exc_type=None, exc_value=None, traceback=None):
        """
        End of a lexing context.
        """
        self.f.close()
        del self.f
    
    def peek(self):
        """
        Return the next character in the buffer, and read more if necessary.
        
        Returns None when no characters left to read.
        """
        ch = None
        while ch is None:
            try:
                ch = self.buffer[0]
            except IndexError:
                # Ran out of characters in the buffer, read some more
                self.buffer = self.f.read(4096)
                if self.buffer == '':
                    # We're out of characters to read, too. :|
                    return None
        return ch
    
    def get(self):
        """
        Just like peek above, but we remove the character from the buffer too.
        """
        ch = self.peek()
        self.buffer = self.buffer[1:]
        return ch
    
    def put(self, string):
        """
        Put something back onto the buffer, useful for rollbacks.
        """
        if string is not None:
            self.buffer = str(string)+self.buffer
            
class CharBuffer:
    def __init__(self, source):
        self.source = source
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type=None, exc_value=None, traceback=None):
        pass
    
    def peek(self):
        try:
            return self.source[0]
        except IndexError:
            return None
    
    def get(self):
        ch = self.peek()
        self.source = self.source[1:]
        return ch
    
    def put(self, string):
        if string is not None:
            self.source += str(string)
    
    
