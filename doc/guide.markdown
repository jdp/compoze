The Compozer's Guide
====================

Introduction
------------

The Compoze programming language is a concatenative language, like Forth, Joy, or Factor. At their core, these languages all have the same semantics: programs are made of composable functions, and function composition in these languages is achieved through concatenation.

Syntax
------

The Compoze syntax is intentionally minimal. It is the result of mixing and matching from existing constructs in other languages, the result being a clean and familiar syntax. At the highest level, a Compoze program is made of **words** and **quotations**.

### Words

A Compoze word, also called a **combinator**, acts like a function in other programming languages, the difference being that its only input and output is the stack. A word applies some transformation to the stack.

For example, the `dup` word places a duplicate of the top-most item on the stack on the stack.

    cz> 9 dup
    # stack is now 9, 9

Another word, the `swap` combinator, swaps the two top-most items on the stack.

    cz> 13 69 swap
    # stack is now 69, 13

### Quotations

A quotation can be thought of as an anonymous function. A quotation is created by surrounding a group of words with square brackets, and they can be nested to arbitrary depth. Once a quotation is created, it is pushed to the stack, and the words and other quotations inside of it are not evaluated.

    [ 13 dup ]

Objects
-------

In Compoze, every value is an **object**. Compoze has a protype-based object system, meaning that every object descends from a single parent object. There is no "class" concept in Compoze, only inheritance.

### Messages

Messages are pieces of data sent to objects that they may respond to.  In truth, words are treated as message sends to the top-most item on the stack. In the rare event that a message is sent to an empty stack, the Dictionary global object receives the message.

Appendix
--------

### Grammar

    program = element (space | element)+ eof
    element = word | quotation
    word = identifier space*
    quotation = '[' (element space*) ']' space*
