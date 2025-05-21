#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Version 0.1

Cílem je vykreslit v "UTF16-artu" strom definovaný listem hodnot. Každý vnitřní uzel stromu obsahuje vždy dvě položky: název uzlu a seznam potomků (nemusí být nutně v tomto pořadí). Názvem může být jakýkoli objekt kromě typu list (seznam).

Příklady validních stromů:
    - triviální strom o 1 uzlu: [1, []]
    - triviální strom o 1 uzlu s opačným pořadím ID a potomků: [[], 2]
    - triviální strom o 3 uzlech: [1, [2, 3]]
        (listové uzly ve stromu o výšce >= 2 mohou být pro zjednodušení zapsány i bez prázdného seznamu potomků)

Příklady nevalidních stromů:
    - None
    - []
    - [666]
    - [1, 2]
    - (1, [2, 3])


Strom bude vykreslen podle následujících pravidel:
    - Vykresluje se shora dolů, zleva doprava.
    - Uzel je reprezentován jménem, které je stringovou serializací objektu daného v definici uzlu.
    - Uzel v hloubce N bude odsazen zleva o N×{indent} znaků, přičemž hodnota {indent} bude vždy kladné celé číslo > 1.
    - Má-li uzel K potomků, povede:
        - k 1. až K-1. uzlu šipka začínající znakem ├ (UTF16: 0x251C)
        - ke K. uzlu šipka začínající znakem └ (UTF16: 0x2514)
    - Šipka k potomku uzlu je vždy zakončena znakem > (UTF16: 0x003E; klasické "větší než").
    - Celková délka šipky (včetně úvodního znaku a koncového ">") je vždy {indent}, výplňovým znakem je zopakovaný znak ─ (UTF16: 0x2500).
    - Všichni potomci uzlu jsou spojeni na úrovni počátku šipek svislou čarou │ (UTF16: 0x2502); tedy tam, kde není jako úvodní znak ├ nebo └.
    - Pokud název uzlu obsahuje znak `\n` neodsazujte nijak zbytek názvu po tomto znaku.
    - Každý řádek je ukončen znakem `\n`.

Další požadavky na vypracování:
    - Pro nevalidní vstup musí implementace vyhodit výjimku `raise Exception('Invalid tree')`.
    - Mít codestyle v souladu s PEP8 (můžete ignorovat požadavek na délku řádků - C0301 a používat v odůvodněných případech i jednopísmenné proměnné - C0103)
        - otestujte si pomocí `pylint --disable=C0301,C0103 trees.py`
    - Vystačit si s buildins metodami, tj. žádné importy dalších modulů.


Příklady vstupu a výstupu:
INPUT:
[[[1, [True, ['abc', 'def']]], [2, [3.14159, 6.023e23]]], 42]

PARAMS:
    indent = 4
    separator = '.'

OUTPUT:
42
├──>1
│...└──>True
│.......├──>abc
│.......└──>def
└──>2
....├──>3.14159
....└──>6.023e+23

INPUT:
[[[1, [[True, ['abc', 'def']], [False, [1, 2]]]], [2, [3.14159, 6.023e23, 2.718281828]], [3, ['x', 'y']], [4, []]], 42]

PARAMS:
    indent = 4
    separator = '.'

OUTPUT:
42
├──>1
│...├──>True
│...│...├──>abc
│...│...└──>def
│...└──>False
│.......├──>1
│.......└──>2
├──>2
│...├──>3.14159
│...├──>6.023e+23
│...└──>2.718281828
├──>3
│...├──>x
│...└──>y
└──>4

INPUT:
[6, [[[[1, [2, 3]], [42, [-43, 44]]], 4], 5]]

PARAMS:
    indent = 2
    separator = ' '

OUTPUT:
6
└>5
  └>4
    ├>1
    │ ├>2
    │ └>3
    └>42
      ├>-43
      └>44

INPUT:
[6, [5, ['dva\nradky']]]

PARAMS:
    indent = 2
    separator = ' '

OUTPUT:
6
└>5
  └>dva
radky

Potřebné UTF16-art znaky:
└ ├ ─ │

Odkazy:
https://en.wikipedia.org/wiki/Box_Drawing
"""

# zachovejte interface metody
# @generated [partially] chatGPT 2025-03-08: Everything related to this context
def update_ctx(ctx, **kwargs):
    """
    Returns a new context updated with the given named parameters
    """
    return (
        kwargs.get("prefix", ctx[0]),
        kwargs.get("indent", ctx[1]),
        kwargs.get("separator", ctx[2]),
        kwargs.get("root", ctx[3]),
        kwargs.get("last", ctx[4])
    )

def process_children(children, ctx):
    """
    Recursively builds and returns a string representation of the list of children nodes
    using the given context
    """
    if not children:
        return ""
    result = ""
    if all(not isinstance(child, list) for child in children):
        for child in children[:-1]:
            branch = '├' + ('─' * (ctx[1] - 2)) + '>'
            result += ctx[0] + branch + str(child) + "\n"
        branch = '└' + ('─' * (ctx[1] - 2)) + '>'
        result += ctx[0] + branch + str(children[-1])
    elif all(isinstance(child, list) for child in children):
        child_lines = []
        for child in children[:-1]:
            c = output_tree(child, update_ctx(ctx, last=False))
            child_lines.append(c)
        c = output_tree(children[-1], update_ctx(ctx, last=True))
        child_lines.append(c)
        result += "\n".join(child_lines)
    else:
        result += output_tree(children, update_ctx(ctx, last=True))
    return result

def process_tree(tree, ctx):
    """
    Recursively processes a tree node represented as a two-element list, where one element
    is the parent node and the other is a list of child nodes, and returns its string representation
    """
    result=""
    if isinstance(tree[0], list) and not isinstance(tree[1], list):
        parent = tree[1]
        children = tree[0]
    elif not isinstance(tree[0], list) and isinstance(tree[1], list):
        parent = tree[0]
        children = tree[1]
    else:
        line1 = output_tree(tree[0], update_ctx(ctx, root=False, last=False))
        line2 = output_tree(tree[1], update_ctx(ctx, root=False, last=True))
        result += line1 + "\n"
        result += line2
        return result.rstrip("\n")
    if ctx[3]:
        line = str(parent)
    else:
        if ctx[4]:
            branch = '└' + ('─' * (ctx[1] - 2)) + '>'
        else:
            branch = '├' + ('─' * (ctx[1] - 2)) + '>'
        line = ctx[0] + branch + str(parent)
    result += line + "\n"
    if ctx[3]:
        new_prefix = ctx[0]
    else:
        if ctx[4]:
            new_prefix = ctx[0] + (ctx[2] * ctx[1])
        else:
            new_prefix = ctx[0] + ('│' + ctx[2] * (ctx[1] - 1))
    new_ctx = update_ctx(ctx, prefix=new_prefix, root=False)
    result += process_children(children, new_ctx)
    return result

def output_tree(tree, ctx):
    """
    Recursively builds a string representation of a tree using the given context
    """
    if not isinstance(tree, list):
        return ctx[0] + str(tree)
    if tree is None:
        return ""
    if len(tree)==2:
        return process_tree(tree,ctx)
    child_lines = []
    for child in tree[:-1]:
        c = output_tree(child, update_ctx(ctx, root=False, last=False))
        child_lines.append(c)
    c = output_tree(tree[-1], update_ctx(ctx, root=False, last=True))
    child_lines.append(c)
    return "\n".join(child_lines)+"\n"

def render_tree(tree: list = None, indent: int = 2, separator: str = ' ') -> str:
    """
    Returns a string representation of the tree with the given indentation and separator
    """
    if not isinstance(tree, list) or len(tree)!=2:
        raise Exception("Invalid tree")
    initial_ctx = ("", indent, separator, True, True)
    res = output_tree(tree, initial_ctx)
    return res.rstrip("\n") + "\n"
