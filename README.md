# jota


### Roadmap

- **done** lexing
- **done** ast building 
- **done** semantic analysis
- **done** bytecode generation
- **done** interpeting
- **soon** native code generation (x86 64)

### Gettin Started:
```
git clone https://github.com/superPuero/jota
cd jota
make
```

### Using compiler
```
./jota jota_src/test.jo -ast -t
```
* **-tokens, -t** dump tokens into stdout
* **-ast** dump abstract syntax tree into stdout
* **-bc** dump bytecode into stdout
* **-interp, -i** run/interperent bytecode
