Tannit 4.0
==========

A fast-cgi based command interpreter for web-based back-end oriented appplications, bringing the power of Object Oriented Programming in C++ directly at the level of the HTML page.

This command interpreter empowers the high-level web-developer with rapid application development scripting instructions to make CRUD operations directly from the web page or any other textual resource.

Tannit commands always take one or more strings as input parameters and return just a string. They are of the form: 

```
  *command_name([parameter, ...])
```  

At run time, the command interpreter replaces the whole command string in the HTML page with the resulting command output string. The Tannit-enriched web-resources (usually HTML or text files) are called Tannit templates.

Tannit command set can also be easily extended by including the header `tnt.h` and sub-classing a specific C++ class, *AbstractCommand*, which is the parent interface for all command-type objects:

```
class AbstractCommand
{
public:
  virtual char* GetName()        = 0;
  virtual char* Execute(char*)   = 0;
  virtual void  Deallocate()     = 0;
}
```

For additional information on the complete list of available Tannit commands please navigate to this repo's [help](https://github.com/gambineri/Tannit/tree/master/help).
