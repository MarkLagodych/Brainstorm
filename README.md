# Brainstorm
## An optimized and extended Brainfuck interpreter
Brainstorm is an easy-to-use library for fast executing an extended version of Brainfuck.
Links:
- https://en.wikipedia.org/wiki/Brainfuck 
- https://esolangs.org/wiki/Brainfuck 

Some optimization ideas:
- https://habr.com/ru/post/321630/

# Syntax
**+ - . , < > [ ]**  do the same things as in Brainfuck.
Any non-command symbols are ignored.

New:
- **`!`** -- works like . but prints the numeric value of current cell;
- **`?`** -- works like , but prompts a number to current cell;
- **`%...%`** -- comment; everything is ignored inside. Example: `+ %+% +` increments current cell only 2 times;
- **`$`** and **`&`** -- virtual console switches; `$` enters console mode and `&` exits the mode.

In virtual console mode all characters are put/got from the command line. After printing a null character to command line, all printed text is treated as command.

Built-in commands:
- `fread x` -- puts contents of file x to the cmd line
- `fwrite x "d"` -- puts d into file x; the double quotes are required and can not be escaped
- `color f b` -- changes console color; f is foreground color; b is background color. Available colors:
    - black
    - red
    - green
    - yellow
    - blue
    - magenta
    - cyan
    - white
    
  On Windows OS also these colors are available:
    - darkblue
    - darkgreen
    - darkcyan
    - darkred
    - darkmagenta
    - darkyellow
    - grey
    - darkgrey


## Example program
```
% This Brainstorm program writes "Hello, world!" into result.txt %
$  % switch to command line mode %
% fwrite result.txt "Hello, world!"\0 %
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++.+++++++++++++++++.-----.-----
----.+++++++++++.---------------.---------------------------------
------------------------------------.+++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++.------------
-.++++++++++++++.++.---------.++++++++.---------------------------
-------------------------------------------.++++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++.++++.----.-------
------------------------------------------------------------------
-----------.++.++++++++++++++++++++++++++++++++++++++.++++++++++++
+++++++++++++++++.+++++++..+++.-----------------------------------
--------------------------------.------------.++++++++++++++++++++
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+.--------.+++.------.--------.-----------------------------------
--------------------------------.+.[-].
&  % switch to console mode %
```