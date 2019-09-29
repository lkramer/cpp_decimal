# C++ Decimal
This is my attempt at creating a decimal type in C++. It allows creation of decimal values with no loss of precision.

The single header filer, `decimal.h` is enough to get started. The CMakeLists.txt is provided to build the gtests.

## Example of use
To create a Decimal type using a string:
```
Decimal val("3.14");
```
With value and exponent:
```
Decimal val(3, 2); // This is equal to 0.03
```
With a double:
```
Decimal val(3.14);
```
Note: Using a double will of course carry over any imprecision and should generally be avoided. The code for parsing doubles is pretty naive, a more optimized version is on my TODO list.
