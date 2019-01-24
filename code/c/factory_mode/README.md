# Fatory_mode
##Purpose
In the parent directory,swan64,swan128 and swan256 are independent,For convenience of test and use,we
made the swan in a single project.
## Usage
### Grap the source
```
git clone https://github.com/GongZheng/SWAN.git
```

### Build steps

```
${build_type} could be none,debug,release
```


| build_type | CMAKE_C_FLAGS |
|:-------------:|:-------------|
| none |  |
|debug|"-O0 -g3"|
| none | "-O2" |


```
cd SWAN/code/c/factory_mode
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=${build_type} ../
make
```


### Run

```
./MAIN
```
