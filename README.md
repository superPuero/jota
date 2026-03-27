# jota


### Roadmap

- **done** lexing
- **done** ast building 
- **done** semantic analysis
- **done** bytecode generation
- **done** interpreting
- **soon** native code generation (x86 64)

### Gettin Started:
```
git clone https://github.com/superPuero/jota
cd jota
make
```

### Using compiler
```
./jota jota_src/example.jo -ast -t
```
* **-dt** dump tokens into stdout
* **-dast** dump abstract syntax tree into stdout
* **-dbc** dump bytecode into stdout
* **-i** run/interperent bytecode
