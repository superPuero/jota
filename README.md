# jota


### Roadmap

- [x] Lexing
- [x] Ast Building 
- [x] Semantic Analysis
- [x] Bytecode gene
- [x] Interpeting
- [ ] Code Generation

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
