from pygments import highlight
from pygments.lexer import RegexLexer
from pygments.token import *
from pygments.formatters import HtmlFormatter
import sys
import os
from lexer import reserved_regex_str, symbols_regex_str

class LocustLexer(RegexLexer):
    name = "Locust"
    aliases = ["locust", "loc"]
    filenames = ["*.loc"]
    
    tokens = {
        'root': [
            (r'\s+', Token.Text.Whitespace),
            (r'\d+.\d+', Token.Literal.Number),
            (r'\d+', Token.Literal.Number),
            (r'.\d+', Token.Literal.Number),
            (reserved_regex_str, Token.Keyword),
            (r'[a-zA-Z]\w*', Token.Name),
            (r'"[^"]*"', Token.Literal.String),
            (r'#.*', Token.Comment),
            (symbols_regex_str, Token.Punctuation),
            ],
    }
    
if __name__ == '__main__':
    ifile = None
    source = ""
    if len(sys.argv) > 1:
        ifile = sys.argv[1] if os.path.isfile(sys.argv[1]) else None
    if ifile is None:
        source = sys.stdin.read()
    else:
        with open(ifile, mode='r') as f:
            source = f.read()
    formatter = HtmlFormatter(linenos=True, cssclass="loc-source")
    result = highlight(source, LocustLexer(), formatter)
    
    with open('out.html', mode='wb') as f:
        f.write("""<!doctype html>
                <head>
                    <style>
                        .hll { background-color: #ffffcc }
                        .c { color: #408080; font-style: italic } /* Comment */
                        .err { border: 1px solid #FF0000 } /* Error */
                        .k { color: #008000; font-weight: bold } /* Keyword */
                        .o { color: #666666 } /* Operator */
                        .cm { color: #408080; font-style: italic } /* Comment.Multiline */
                        .cp { color: #BC7A00 } /* Comment.Preproc */
                        .c1 { color: #408080; font-style: italic } /* Comment.Single */
                        .cs { color: #408080; font-style: italic } /* Comment.Special */
                        .gd { color: #A00000 } /* Generic.Deleted */
                        .ge { font-style: italic } /* Generic.Emph */
                        .gr { color: #FF0000 } /* Generic.Error */
                        .gh { color: #000080; font-weight: bold } /* Generic.Heading */
                        .gi { color: #00A000 } /* Generic.Inserted */
                        .go { color: #808080 } /* Generic.Output */
                        .gp { color: #000080; font-weight: bold } /* Generic.Prompt */
                        .gs { font-weight: bold } /* Generic.Strong */
                        .gu { color: #800080; font-weight: bold } /* Generic.Subheading */
                        .gt { color: #0040D0 } /* Generic.Traceback */
                        .kc { color: #008000; font-weight: bold } /* Keyword.Constant */
                        .kd { color: #008000; font-weight: bold } /* Keyword.Declaration */
                        .kn { color: #008000; font-weight: bold } /* Keyword.Namespace */
                        .kp { color: #008000 } /* Keyword.Pseudo */
                        .kr { color: #008000; font-weight: bold } /* Keyword.Reserved */
                        .kt { color: #B00040 } /* Keyword.Type */
                        .m { color: #666666 } /* Literal.Number */
                        .s { color: #BA2121 } /* Literal.String */
                        .na { color: #7D9029 } /* Name.Attribute */
                        .nb { color: #008000 } /* Name.Builtin */
                        .nc { color: #0000FF; font-weight: bold } /* Name.Class */
                        .no { color: #880000 } /* Name.Constant */
                        .nd { color: #AA22FF } /* Name.Decorator */
                        .ni { color: #999999; font-weight: bold } /* Name.Entity */
                        .ne { color: #D2413A; font-weight: bold } /* Name.Exception */
                        .nf { color: #0000FF } /* Name.Function */
                        .nl { color: #A0A000 } /* Name.Label */
                        .nn { color: #0000FF; font-weight: bold } /* Name.Namespace */
                        .nt { color: #008000; font-weight: bold } /* Name.Tag */
                        .nv { color: #19177C } /* Name.Variable */
                        .ow { color: #AA22FF; font-weight: bold } /* Operator.Word */
                        .w { color: #bbbbbb } /* Text.Whitespace */
                        .mf { color: #666666 } /* Literal.Number.Float */
                        .mh { color: #666666 } /* Literal.Number.Hex */
                        .mi { color: #666666 } /* Literal.Number.Integer */
                        .mo { color: #666666 } /* Literal.Number.Oct */
                        .sb { color: #BA2121 } /* Literal.String.Backtick */
                        .sc { color: #BA2121 } /* Literal.String.Char */
                        .sd { color: #BA2121; font-style: italic } /* Literal.String.Doc */
                        .s2 { color: #BA2121 } /* Literal.String.Double */
                        .se { color: #BB6622; font-weight: bold } /* Literal.String.Escape */
                        .sh { color: #BA2121 } /* Literal.String.Heredoc */
                        .si { color: #BB6688; font-weight: bold } /* Literal.String.Interpol */
                        .sx { color: #008000 } /* Literal.String.Other */
                        .sr { color: #BB6688 } /* Literal.String.Regex */
                        .s1 { color: #BA2121 } /* Literal.String.Single */
                        .ss { color: #19177C } /* Literal.String.Symbol */
                        .bp { color: #008000 } /* Name.Builtin.Pseudo */
                        .vc { color: #19177C } /* Name.Variable.Class */
                        .vg { color: #19177C } /* Name.Variable.Global */
                        .vi { color: #19177C } /* Name.Variable.Instance */
                        .il { color: #666666 } /* Literal.Number.Integer.Long */
                    </style>
                </head>
                <body>"""+result+"</body>")