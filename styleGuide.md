* Tab size: 4
* Variable and function names: camelCase
* Defined constant names: uppercase and underscore between words
* Ifndef headers files: NAME_FILE_H
* Space between variables and operators, but no space after/before parentheses.
    Example: (var1 + var2)

* Braces on top and space between parentheses - brace
    Example:
        if(x == "styleGuide") {
            printf("Remember the space!");
        }

* Pointer types variables: asterisk next to variable name
    Example:
        int *variable = value;
        int *functionName(int *var1) {
            var1 = (int *) 0;
            printf("Hello World!");
        }

    Except VOID:
        void * variable;
        void * functionName(void * var1);
