Code Style {#code_style}
===========================================================

Overview {#code_style_overview}
===========================================================

General advice: Follow existing practice of the code around you.

The following code example summarizes most of the style:

    #include <math.h>
    #include "cxBar.h"

    namespace cx
    {

    /** A Complex Number.
    * 
    * Representation of a complex number consisting of an 
    * imaginary and a real part.
    */
    class ComplexNumber
    {
    public:
      ComplexNumber(double re, double im)
        : mRe(re), mIm(im)
      {}
      double getModulus() const
      {
        return sqrt(mRe*mRe + mIm*mIm);
      }
    private:
      double mRe;
      double mIm;
    };

    void foo(int a, int b)
    {
      for (int i=0; i<a; i++)
      {
        if (i < b)
          bar(i);
        else
        {
          bar(i);
          bar(b);
        }
      }
    }

    } // namespace cx

Code Style 
-----------------------------
3. Normally use block grouping (ie { }) for if, while, for and do statements. 
   Brackets are always on their own line, the first one indented similar to the 
   preceeding code. 
4. Add a space after syntax elements like if, while, do and so on. Generally, 
   if in doubt, add space. 
5. Use tab indentation for new lines of code, where tabs represent 2 or 4 
   spaces, except for tabulating continued lines, where spaces should be used. 
   Users are free to chose the tab length they want.

Names 
-----------------------------
1. All comments and symbol names shall be in English.
2. Filenames have the style \<moduleprefix\>\<classname\>.cpp/h, for example cxVector3D.h 
3. Class names are camel case with first uppercase letter.
4. Member variables in a class are usually prefixed with "m". The rest of the name is in camel case. 
5. Use accessors named getCamelCase() for get, setCamelCase() for set.. 
6. All functions shall be named in lowerCamelCase(). 
7. Smart pointer typedefs append Ptr to the class name. e.g. typedef boost::shared_ptr FooPtr; 
8. All new libraries shall have its name prefixed with the namespace. Example: cxUtilities.so 

Comments
-----------------------------
1. All code references in a header file shall be documented using javadoc-style 
   Doxygen comments. This means /** ... text ... */. 
2. Comments should explain how something works when this is difficult to 
   understand, and why an approach was chosen when this is not obvious. 
3. Focus on documenting classes and public methods. This is what defines the 
   class to others. 

Structure 
-----------------------------
1. Write platform-independent code. The code will be run on Linux, Windows and MacOSX.
2. Remove unused code from production code. Do not litter the code with commented out code sections. 
3. Do not use globals. 
4. Use pointer ('*') rather than reference ('&') to pass a return value from a function. 
5. Avoid the use of the operator delete, use Qt/VTK-specific allocation methods, or smart pointers (e.g. boost::shared_ptr).
6. Use const where possible.
7. Organize includes starting with the most general library down to the closest neighbours.
8. Do not add unnecessary headers in a header file, use forward declarations instead. 
9. .cpp files always include their own header file first. 
10. Create unit tests for all new code. 

Numerical data 
-----------------------------
1. Double precision ('double') floating point shall be used unless performance 
   is critical and tested to perform better with single precision. 
2. Distances/Spatial data are given in millimeters, unless explicitly specified. 
3. Angles are given in radians, unless explicitly specified. 
4. Time are given in milliseconds, unless explicitly specified. 
5. Ratios are given in natural ratios [0-1] instead of percent [0-100] or 
   bytes [0-255], unless explicitly specified.

Warnings 
-----------------------------
1. All projects shall compile and link without warnings. 
2. Compile the code with as many warning flags as possible to catch errors. Always use the -Wall and -Wformat-security flags. 

Error handling and logging
-----------------------------
2. Normally, the top-level code does not catch exceptions. 
1. After an error occurs, the module should be in a consistent state (no resource leaks, accepts input etc). 
3. Wrap calls that might throw exceptions within a try..catch block, unless you want to crash the process. 

Modules/namespaces 
-----------------------------
1. In general, use only existing namespaces cx and cxtest. Add more namespaces sparingly.
3. All namespaces shall be in lower case letters. 
4. Everything shall exist inside a namespace, except catch tests.
6. Never use 'using' in header files. Use with care in cpp files. 
   
Usage in IDE's 
-----------------------------
1. When using Qt Creator, use Qt style, modified with 'tabs only'.
2. When using Eclipse, set Code Style to BSD/Allman, line length 120. Format code using Ctrl-Shift-F. 
   
