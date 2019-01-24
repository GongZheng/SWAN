# SWAN
The source codes of the SWAN cipher

SWAN: Substitution-based White-box oriented block cipher with Affine Network

Software implementation of cryptographic algorithms are widely-used in practice due to its low costs and high interoperability. Among those cryptographic algorithms, block ciphers are mostly implemented for data integrity and confidentiality. The seminal idea of white-box block ciphers was first introduced by Chow et al, which is focusing on how to protect secret keys not to be revealed by fully controlled adversaries. Unfortunately, to the best of our knowledge, the published proposals of white-box block ciphers suffer many theoretical and practical issues. In this talk, a new substitution-based white-box oriented block cipher with affine network (which is named SWAN) is proposed. The design rationale of SWAN not only considers the theoretical issues, but also the implementation costs. The block and key sizes of SWAN support 64/128/256 and 128/256 respectively, which are compatible for various platforms.

## Fatory_mode usage

### Grap the source
```
git clone https://github.com/GongZheng/SWAN.git
```

###Build steps

```
${build_type} could be none,debug,release
```
*none*
*debug    CMAKE_C_FLAGS   "-O0 -g3"*
*release  CMAKE_C_FLAGS "-O2"*
```

```
cd SWAN/code/c/factory_mode
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=${build_type} ../
make
```


