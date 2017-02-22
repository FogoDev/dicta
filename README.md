# Dicta

Dicta (utterances in latim) is a open source project of vocal utterances recognition. Based on Weightless Neural Network Model WiSARD (Wilkes, Stonham and Aleksander Recognition Device), it aims to be a general purpose library to the recognition of any set of vocal utterances from a single user.

### Requirements

#### CMake >= v3.5
Dicta uses [CMake](https://cmake.org) as the project's build system


####libsoundio
Dicta uses [libsoundio](https://github.com/andrewrk/libsoundio) to get audio data through a input device, to install it just follow the instructions on libsoundio github page.

### Instructions


```
git clone https://github.com/fogodev/dicta.git
cd dicta
cmake .
make
./Dicta
```
