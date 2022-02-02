# wordle-solver
Interactive CLI to look up word options for the Wordle game

## Example
```
# wordle-solver dictionary.txt 

Wordle Solver

Commands:
  hint <word> <colors> Add a hint. The first argument is the 5 letter word.
                       The second argument is the initials of the colors of each letter box.
                       For example, "bbbgy" is black, black, black, green, yellow.
  list                 Print a list of possible words
  info                 Print dictionary info

Enter a command:
> hint train GBBBB
> hint towel GYBYB
> list 
3 options:
  tempo
  tepoy
  those
> 

```
