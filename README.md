# Real time systems engineering
## Repo structure
All the exercises are in the exercises folder, with the structure ```<#exercise>/<#part>.cpp```. Each exercise's solution is implemented independently in order to provide a standalone solution.
## How to use the code ?
To try the code, compile with one the following line :
```sh
# For files in the exercises/1/ folder
g++ -lrt <file>
# For files in the exercises/2/ folder
# If using Ubuntu 17
g++ -lrt -pthread <file>
# In most cases
g++ -lrt -lpthread <file>
```
