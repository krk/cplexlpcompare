cplexlpcompare
==============

Compares two LP files created in cplex format and dumps differences to files.

Requirements
============
* CMake - Used to generates sln files on Windows and makefiles on Linux.
* Boost - Used to access files using memory mapping.


CMake must be able to find the address-mode that Boost was built, 32-bit or 64-bit, that corresponds to the CMake generator.

Tested On
=========
* Win x86-64 MSVS 2013
* Win x86-32 MSVS 2013

Usage
=====
```
  --help                        show usage information
  --first arg                   model 1 cplex lp file
  --second arg                  model 2 cplex lp file
  --dump-prefix arg (=diffdump) filename prefix for difference dumps
  --dump-diffs arg (=1)         filename prefix for difference dumps
```

Example Usage
=============

```
lpcompare c:\model1.lp c:\model2.lp 
```

```
lpcompare: Compares two LP files created in cplex format and dumps differences to files.
Reading first model: C:\model1.lp
Lines read: 1000000
 First model:
Binaries: 1
Bounds: 102561
Constraints: 124370
Generals: 0
SosVars: 0
 Model read in 47 s
Reading second model: C:\model2.lp
Lines read: 1000000
 Second Model:
Binaries: 1
Bounds: 102561
Constraints: 124370
Generals: 0
SosVars: 0
 Model read in 44 s

Generals First Model: 0
Generals Second Model: 0
Generals are equivalent.
Binaries First Model: 1
Binaries Second Model: 1
Binaries are equivalent.
SosVars First Model: 0
SosVars Second Model: 0
SosVars are equivalent.
Bounds First Model: 102561
Bounds Second Model: 102561
Bounds are equivalent.
 Bounds check completed in 5 s
Constraints First Model: 124370
Constraints Second Model: 124370
Constraints First except Second: 108
Constraints Second except First: 108
firstEXCEPTsecond diff written for Constraints to diffdump-firstEXCEPTsecond-Constraints.log
secondEXCEPTfirst diff written for Constraints to diffdump-secondEXCEPTfirst-Constraints.log
 Constraints check completed in 54 s
```
