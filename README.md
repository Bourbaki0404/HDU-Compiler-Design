# Mini Cpp Compiler 

### QuickStart

> 1. The source program should be written at test.input.c
> 2. Use <b>make test</b> for testing the compiler.
> 3. All production rules are defined at parser/parser.cpp.

### Features:

> 1. Handcrafted scanner.
> 2. Rule-action style LR(1) parser with conflict resolution.
> 3. Tree representation of program and statics checker with graceful error handling.
> 4. Support preliminary oop and pointer type.
> 5. LLVM-IR for intermediate codeGen.

### ToDos:

> 1. Class inheritance and dynamic dispatch.
> 2. Support other primitive datatypes such as bool, float and char.
> 3. RISC-V backend, static analyzer and intermediate code optimizer.
> 4. Function overloading and type inference.
> 5. Automatic memory management (garbage collection) for objects (specified by programmer).
> 6. More syntatic support like typedef, lambda expression or namespace.

### References:
- Overview
> DragonBook chapter 1, 2, 3, 6 :https://www3.nd.edu/~dthain/compilerbook/compilerbook.pdf

- FrontEnd
> 1. Write a C compiler: https://tutorials.ophion.feralhosting.com/Library/Ebooks/No%20Starch%20Press/Writing%20a%20C%20Compiler%20-%20Build%20a%20Real%20Programming%20Language%20from%20Scratch/Writing%20a%20C%20Compiler%20-%20Build%20a%20Real%20Programming%20Language%20from%20Scratch.pdf
> 2. Parser: http://staff.ustc.edu.cn/~yuzhang/compiler/2021f/lectures/03-parsing.pdf
> 3. AST: https://github.com/llvm/llvm-project/tree/main/clang/include/clang/AST
> 4. TypeChecker: https://www.bilibili.com/video/BV1sgH3eHEtS?spm_id_from=333.788.videopod.episodes&vd_source=13bf89d4f930b74c0c5255c55db8f768&p=2

- MiddleEnd
> 1. How to write a LLVM Pass: https://github.com/banach-space/llvm-tutor?tab=readme-ov-file#helloworld-your-first-pass 
> 2. CMU - Optimizting Compiler: https://www.cs.cmu.edu/afs/cs/academic/class/15745-f24/www/lectures.html
> 3. Nju - Software Analysis: https://static-analysis.cuijiacai.com/preface/
> 4. Nju Assignment: https://tai-e.pascal-lab.net/lectures.html
> 5. Cornell - Advanced Compiler: https://www.cs.cornell.edu/courses/cs6120/2025sp/


- BackEnd
> 1. RISC-V BackEnd Slide: https://speakerdeck.com/asb/llvm-backend-development-by-example-risc-v?slide=8</br>
Video: https://www.youtube.com/watch?v=AFaIP-dF-RA
> 3. SelectionDAG Video: https://www.youtube.com/watch?v=nNQ6AF6i5FI </br>
> 4. LLVM Backend: https://sourcecodeartisan.com/2020/11/17/llvm-backend-4.html
