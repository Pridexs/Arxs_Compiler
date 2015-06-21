# Arxs Compiler  
Authors: Alexandre Maros & Aurelio Grott Neto  
This is a Compiler for the Jasmin assembler. It was initially made for my Compilers class but is slowly being expanded. It has a very simple language syntax.  
The code is mostly in the Brazilian Portuguese language. But I might change that in the near future.  

# Dependencies
* Flex
* Bison
* C

# Rules
Some rules of the language
* You must declare all your functions at the top of the { } block.
* Variables **must** be declared at the start of the block, not in the middle of the code.
* There is no global variables.
* There can't be an 'if' or 'while' with nothing in it.
* There's only two types for variables. Strings and Integer. Functions have the void type.
* 
You can learn more about the language in the flexbison/expr.y file.

# Examples
Here is an example code:
```c
int max(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

{
    int i,j, u;

    print("Type number a:");
    read(i);
    print("Type number b:");
    read(j);

    u = max(i, j);
    print(u);
}
```
This code will generate a name_of_file.jout in the corresponding Jasmin syntax.

# Compiling and Running
Run the 'make' command in your terminal. Then, run ./arxs name_of_file.extension and it will generate the file if everything is correct.  
If you want to run the compiled code, download Jasmin from http://jasmin.sourceforge.net/ and run 'java -jar jasmin.jar name_of_file.jout' This will generate a .class that can be run normally (hopefully).

# Todo List
* Check if the attributes of a function call are all correct.
* Add comments
* ... (add more)
