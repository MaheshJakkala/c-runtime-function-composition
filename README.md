# 🚀 C Runtime Function Composition Engine

A low-level runtime function composition system implemented in **C** that extracts compiled **x86 machine code**, manually stitches instruction streams in **executable memory**, and invokes the resulting composed function.

This project demonstrates:

- Runtime code generation  
- Executable memory allocation via `mmap`  
- Binary-level function manipulation  
- Manual control over machine instructions  
- Dynamic function invocation through function pointers  

---

## 🧠 Concept

Given two functions:

```c
int addone(int x);
int x2(int x);

```
The engine synthesizes a new function at runtime equivalent to:

int composed(int x) {
    return x2(addone(x));
}

Instead of calling functions normally, it:

1. Extracts raw machine code from both functions
2. Removes the RET instruction from the first function
3. Allocates executable memory (PROT_EXEC)
4. Copies and concatenates instruction streams
5. Bridges register/stack flow explicitly
6. Executes the generated function via a function pointer

## 🏗 Architecture

### 1️⃣ Machine Code Extraction

* Scans function memory until RET (`0xC3`)

* Copies instruction bytes into a buffer

### 2️⃣ Executable Memory Allocation

mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, ...)

* Enables runtime execution of synthesized code

### 3️⃣ Binary Stitching
[f1 machine code] → [register bridge] → [f2 machine code]

### 4️⃣ Invocation
```
ret.call = (int (*)(int)) fun;
```

## 🛠 Build & Run
gcc -Wall -Wextra -O2 src/composer.c examples/basic_example.c -o compose
./compose

## ⚠️ Platform Notes

* Designed for **x86 Linux**
* Assumes specific calling conventions
* Not portable across architectures
* Intended as an educational, systems-level experiment

## 🔬 What This Demonstrates

* Calling convention awareness
* Stack frame mechanics
* Low-level memory layout understanding
* Executable memory management (`mmap`)
* Binary instruction-level reasoning
* Core runtime code generation principles

## 🚧 Limitations
* Architecture dependent
* No relocation handling
* No instruction rewriting
* No safety sandboxing
* Assumes simple function layouts
