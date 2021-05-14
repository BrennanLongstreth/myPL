# MyPL language  
### About the Language  
Language grammar was defined by Dr. Bowers at Gonzaga University for CPSC 326 Programming Languages. It is a procedural language that writes somewhat similar to python. As part of the class our job was to implement the majority of a compiler for MyPL, this included parsing the incoming file into a token stream and building an abstract syntax tree for which to type check. After this the program runs in C++. As a final project we were to add a plugin for MyPL of our choice.  

# Project Description  
### C++ Translator 
#### What worked:
 * Conversion to c++ for functions, variables, expressions, loops, recursion, UDT

#### What didn't work:
* Conversion to C++ for the ability to set any variable to nil.
* I couldnt get the translator to setup static casting on the last term of a concatonation expression, which isnt a problem in most cases but there are a few awkward spots. (if i make it static cast the final term it starts doing it too often and consistently breaks conditional statements.)
  
#### Testing:
* I used the same testing approach as hw6 reusing the tests from there
* Only modification was to the cond.mypl program to remove the undefined behavior of setting x =nil

#### Plans for the future:
* I think it would be interesting to add defined behavior for some of the other 
extensions like the list and classes so that the translator worked with those as well.
* I also would like to write some longer programs in MyPL to stress my interpreter a bit more and maybe give the c++ compiler an advantage.

#### Instructions for building and running:
run "cmake CMakeLists.txt" to generate make files.  
run "make" to build the compiler executables.  
run "./mypl <FILE_NAME>" to run a file directly.  
run "./mypltocpp <FILE_NAME>" to run convert the file to c++ and run.  
C++ files are stored in "test.cpp".  
