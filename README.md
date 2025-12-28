
# libsinex

A library to handle Solution (Software/technique) INdependent EXchange Format, SINEX.
For an detailed description of SINEX, checkout the relevant 
[IERS webpage](https://www.iers.org/IERS/EN/Organization/AnalysisCoordinator/SinexFormat/sinex.html).
Latest version handled is 
[Version 2.02 (December 01, 2006)](https://www.iers.org/SharedDocs/Publikationen/EN/IERS/Documents/ac/sinex/sinex_v202_pdf.pdf;jsessionid=D3307FA604FD12ACBE02A30C3D9AFC30.live1?__blob=publicationFile&v=2)


[![clang-format Check](https://github.com/xanthospap/libsinex/actions/workflows/clang-format-check.yml/badge.svg)](https://github.com/xanthospap/libsinex/actions/workflows/clang-format-check.yml)
[![Linux CI build](https://github.com/xanthospap/libsinex/actions/workflows/cpp-linux-build.yml/badge.svg)](https://github.com/xanthospap/libsinex/actions/workflows/cpp-linux-build.yml)

## Dependencies
To install and use the library, you will need:

* [ggdatetime](https://github.com/xanthospap/ggdatetime) and
* [ggeodesy](https://github.com/xanthospap/ggeodesy)

## Installation

```
## to build in a folder named "build":
$> cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/local/lib
$> cmake --build build --target all --config Release
## (Optional) run tests, see section Testing below
$> ctest --test-dir build
## Install, system-wide (needs root)
$> cd build && sudo make install
```

## Developers
Take a look at the [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html) 
and if possible stick to it. 

Use [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
to format you code before commiting; if you try to commit with non-acceptable 
code format, the CI system will fail.

### Testing
Test source code is found in [test folder](https://github.com/xanthospap/libsinex/tree/main/test). 
To run the tests (via `ctest`) you will need the `dpod2020_023.snx` file. For 
more information, including the download links, see the relevant [IDS page](https://ids-doris.org/combination/dpod.html). 
Download the sinex file, and place it in a folder named `data` under at the main 
directory tree. Then, you can use `ctest --test-dir build` to run the tests.

## ToDo

- [] More tests (e.g. extrapolate_coordinates, etc)
- [] Handle Psd
- [] Handle harmonic signals
- [] Handle geocenter motion model
- [] Adapt ITRF2020

## PREPARE Funding
>This study was funded under the project PREPARE (HFRI PN:15562) in the framework of H.F.R.I call “Basic research Financing (Horizontal support of all Sciences)” under the National Recovery and Resilience Plan “Greece 2.0” funded by the European Union – NextGenerationEU<br>
<img src="https://github.com/demanasta/prepare_web/blob/main/logos/prepare_logo.png" width="150"><img src="https://github.com/demanasta/prepare_web/blob/main/logos/ntua.png" width="150"><br>
<img src="https://github.com/demanasta/prepare_web/blob/main/logos/elidek_en.jpg" width="200">  <img src="https://github.com/demanasta/prepare_web/blob/main/logos/Gr20_en.jpg" width="380">


