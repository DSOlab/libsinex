# libsinex

[![Build Status](https://app.travis-ci.com/xanthospap/libsinex.svg?branch=main)](https://app.travis-ci.com/xanthospap/libsinex)

A library to handle Solution (Software/technique) INdependent EXchange Format, SINEX.
For an detailed description of SINEX, checkout the relevant 
[IERS webpage](https://www.iers.org/IERS/EN/Organization/AnalysisCoordinator/SinexFormat/sinex.html).
Latest version handled is [Version 2.02 (December 01, 2006)](https://www.iers.org/SharedDocs/Publikationen/EN/IERS/Documents/ac/sinex/sinex_v202_pdf.pdf;jsessionid=D3307FA604FD12ACBE02A30C3D9AFC30.live1?__blob=publicationFile&v=2)

## Dependencies
To install and use the library, you will need:

* [ggdatetime](https://github.com/xanthospap/ggdatetime) and
* [ggeodesy](https://github.com/xanthospap/ggeodesy)

## Installation
Installation is straightforward, using the [scons](https://scons.org/) build system. You will need a C++17 compliant compiler. 
To install, just type:
```
## build ...
scons
## install ...
sudo scond install
```

When building, you can use the `debug=1` option to trigger a `DEBUG` build:
```
scons debug=1
```
to build the `DEBUG` version.

## Examples
Consult the test programs source code in the [test folder](https://github.com/xanthospap/libsinex/tree/main/test) for examples on how to use the library.
