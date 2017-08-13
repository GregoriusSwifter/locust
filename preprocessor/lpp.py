#!/usr/bin/env python

## sample magic macro commands
# X = "Y"
# X(a, b, c) = "(Y[a], Y[b], Y[c])"
# X = nix
# if defined(X) {
# } else {
# }

## builtin macro values
# __argc
# __

## builtin macro functions
# __defined   - Evaluates to true if argument is defined
# __import    - Includes a file, only once.
# __run       - Runs the text on the command line and puts output in its place

## user facing versions -- So implementations can change without use needing to.
# defined(X)
# import(X)
# run(X)

##

X = "Y"
Y = "Z"
Z(bacon, twenty) = "herp" "X" "derp"
X = nix

if defined(X) {
    X = "y" Y = "c"
} else {
    X = "z"
}
##
