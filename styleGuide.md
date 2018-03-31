* Tab size: 4
* Max characters per line: 80 - Cut the line after an operator or comma
* Variable and function names: camelCase
* Defined constant names: uppercase and underscore between words
* Ifndef headers files: NAME_FILE_H
* Space between variables and operators, but no space after/before parentheses.

    * Example:
    ```c
    int x = (var1 + var2);

    int z = ((2 * x) + y);
    ```

* Space after comma

    * Example:
        ```c
        x = functionCall(param1, param2, param3);

        int function(char c, int n) {
            printf("Note space next to char c");
        }
        ```

* Braces on top and space between parentheses - brace

    * Example:
        ```c
        if(x == "styleGuide") {
            printf("Remember the space!");
        }
        ```

* Pointer types variables: asterisk next to variable name

    * Example:
        ```c
        int *variable = value;

        int *functionName(int *var1) {
            var1 = (int *) 0;
            printf("Hello World!");
        }
        ```

    * Except VOID:
        ```c
        void * variable;

        void * functionName(void * var1);
        ```

* One-line sentences on if/loop: with Braces

    * Example:
    ```c
    if(x == NULL) {
        exit(1);
    }
    else {
        printf("Cool example!\n");
    }

    while(i < 10) {
        printf("10 Magic number? %d\n", i);
    }
    ```

* Do-While loop: while next to bottom brace (with space)

    * Example:
    ```c
    do {
        something(x++);
    } while(x < y);
    ```

* Loop without code: semicolon next to parentheses
    * Example:
    ```c
    while(x == NULL);

    for(int i = 0; i < 10; i++);
    ```
