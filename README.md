# README for Assigment 7

## How to compile
Use Makefile, type make
```bash
make
```

For infer check, type make infer
```bash
make infer
```

## How to use
### encode
./encode [-h] [-v] [-i infile] [-o outfile]

-h: Show help information

-v: Display compression statistics

-i <input>: Specify input to compress (stdin by default)

-o <output>: Specify output of compressed input (stdout by default)

Example:
```bash
./encode -v -i bible.txt -o bible
```

### decode
./decode [-h] [-v] [-i infile] [-o outfile]

-h: Show help information

-v: Display compression statistics

-i <input>: Specify input to decompress (stdin by default)

-o <output>: Specify output of decompressed input (stdout by default)

Example:
```bash
./encode -v -i bible -o bible-decompressed.txt
```
