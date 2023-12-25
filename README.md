# AutoProve
This is a toy, which can automatically make first order logic proof, for limitied situations.

## Usage
AutoProve.exe <input_file>

## Input File Format
write propositions line by line, and the last line should be the conclusion to prove
- symbols shoud be "[A-Z][0-9]*"
- `v` is "or"
- `^` is "and" 
- `->` is "entails"
- `<->` is "equivalent"
- `/\` means "contradiction"

For example:
```
(A v C) -> B
C
B
```
defines the problem
```
(A v C) -> B, C |- B
```
See tests/test.txt for another example.

## Working Example
### input
```
(!P -> Q) v !R
(P v Q) -> S ^ !Q
R
P ^ R
```
### result
```
-----proof-----
0              (0)     ((!P->Q)v!R)                             P
1              (1)     ((PvQ)->(S^!Q))                          P
2              (2)     R                                        P
0              (3)     (R->(!P->Q))                             P-alt
4              (4)     !P                                       ASP!
0,2            (5)     (!P->Q)                                 3,2 ->E
0,2,4          (6)     Q                                       5,4 ->E
0,2,4          (7)     (PvQ)                                   6 vI
0,1,2,4        (8)     (S^!Q)                                  1,7 ->E
0,1,2,4        (9)     S                                       8 ^E
0,1,2,4        (10)    !Q                                      8 ^E
0,1,2,4        (11)    (S^!Q)                                  9,10 ^I
0,1,2,4        (12)    !Q                                      11 ^E
0,1,2,4        (13)    /\                                      6,12 !E
0,1,2          (14)    P                                       4,13 !I
0,1,2          (15)    (P^R)                                   14,2 ^I
```

## Limitations
Notice that I have mentioned that this is just a toy. It at least has the following limitations:
- **memory leak**: it **never** releases the memory of the raw pointers it allocates.
- limited number of recursive calls
- limited number of negative assumptions
- not able to solve some kind of problems even if you think it should

