#!/usr/bin/env python
from collections import deque

def as_c(node):
    """Generator that incrementally collapses an AST as C code, given any node.
    
    AST nodes can return a string, a callable, or sequence of any number thereof
    """
    jobs = deque()
    jobs.append(node.as_c)
    while True:
        try:
            result = jobs.popleft()
        except IndexError:
            raise StopIteration
        while callable(result):
            # I sure hope no AST node get cute and decide to return itself.
            result = result()
        if isinstance(result, str):
            yield str(result)
        else:
            # Assuming sequence
            jobs.extendleft(reversed(result))
            # Must reversed() -->  extendleft puts in elements backwards.

