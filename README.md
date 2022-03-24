# smihelp - a small file manipulation helper 

Facing obstacles while using SMILES related files, I decided to automate as much of it as possible, thus justifying the very existence of this repository.  
This program doesn't have one particular goal in mind, instead it is decorated with solutions to the different problems we face, and might not make sense.

Setting up the program : (untested)
- `git clone https://github.com/hihubbIe/smihelp.git`
- `cd smihelp`
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

Program arguments :
- `--help` : show help
- `--rmempty` : remove empty lines
  - `--in <file in>` : input file
  - `--out <file out>` : output file
- `--tab2smi` : convert tab file to smi
  - `--in <file in>` : input file
  - `--out <file out>` : output file
  - `--format <format>` : columns to keep (e.g. "3,1" : columns 3 then 1)
  - `--delim <delimiter>` : delimiter to write between columns

- `--debug` : activate debugging logs (this parameter can be used after the mode and between any other parameter, but not between a parameter name and its value (e.g. `--in --debug "myfile.smi` is not correct).

## --rmempty

Remove every line that is either empty, or composed of white space or tabulations.  
Example : `./smihelp --rmempty --in molecules.smi --out molecules_clean.smi`

## --tab2smi

Format a tab separated values data file ordered in columns in another order, with a personal delimiter.  
Example : `./smihelp --tab2smi --in molecules.tab --out molecules.smi --format "3,1" --delim "\t"`

## --embbedsmisdf

**Not implemented yet**

Embbed molecules from a **.smi** file in a **.sdf** file.  
Example : `./smihelp --embbedsmisdf --sdf molecules.sdf --smi molecules.smi --out molecules_embbeded.smi`

___
Enjoy ! 
