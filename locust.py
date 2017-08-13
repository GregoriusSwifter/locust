#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os.path
import logging
import argparse

from compiler.module import Module
from compiler.lexer import Lexer
from compiler.parser import Parser
from compiler.utils import FileCharBuffer
from compiler.ast.collapser import as_c

if __name__ == '__main__':
    from compiler import settings
    def main():
        arg_parser = argparse.ArgumentParser(version=settings.version)
        arg_parser.add_argument("-d", "--debug", action="store_true",
                              dest="debug", default=False,
                              help="Run in debug mode.")
        arg_parser.add_argument('infile', type=argparse.FileType('r'), help='file to begin parsing')
        args = arg_parser.parse_args()
    
        logging_level = logging.WARN
    
        if args.debug:
            logging_level = logging.DEBUG
    
        log_dir = os.path.join(os.path.dirname(__file__), 'logs')
        log_file = os.path.join(log_dir, 'debug.log')
        if not os.path.isdir(log_dir):
            os.mkdir(log_dir)
    
        logging.basicConfig(filename=log_file,
                            filemode='w',
                            level=logging_level)
        log = logging.getLogger()
    
        # Reading from a file.
        
        root = Module(args.infile, args.infile.name)
        lexer_ = Lexer(root, FileCharBuffer(args.infile))
        parser_ = Parser(root, lexer_)

        for node in parser_.nodes():
            for string in as_c(node):
                if string is None:
                    exit = True
                    break
                print(string)
            if exit is True:
                break

if __name__=='__main__':
    exit(main())
    
"""
#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os.path
import logging
import optparse

from compiler import settings, parser, lexer, module

def main():
    opt_parser = optparse.OptionParser(usage=settings.usage,
                                       prog=settings.name,
                                       version=settings.version)
    opt_parser.add_option("-d", "--debug",
                          action="store_true",
                          dest="debug",
                          default=False,
                          help="Run in debug mode.")
    (options, args) = opt_parser.parse_args()

    logging_level = logging.WARN

    if options.debug:
        logging_level = logging.DEBUG

    log_dir = os.path.join(os.path.dirname(__file__), 'logs')
    log_file = os.path.join(log_dir, 'debug.log')
    if not os.path.isdir(log_dir):
        os.mkdir(log_dir)

    logging.basicConfig(filename=log_file,
                        filemode='w',
                        level=logging_level)
    log = logging.getLogger()

    # Reading from a file.
    if args:
        input_file = args[0]
    else:
        print("Specify an input file.")
        exit(0)

    print("using "+input_file)
    # Get the representative module of the source file.
    m = module.Module(input_file, 'main', lexer.Lexer, parser.Parser)

if __name__=='__main__':
    main()
"""
