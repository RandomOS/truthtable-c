## Usage
truth_table [FILE]

## Description
Truth table calculator written in ANSI C

* `!`: not
* `&`: conjunction
* `|`: disjunction
* `>`: implication
* `#`: equivalence
* `A-Z`: element

## Options
FILE
    read FILE, each line is an expression, print each true table in line.

## Compile
    gcc truth_table.c -lm -o truth_table

## Test
    truth_table[.exe] example.txt > result.txt