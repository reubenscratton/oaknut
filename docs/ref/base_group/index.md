---
layout: default
---

# Base

Core stuff used by everything else, includes the Object base class (and smart pointer helper), timers, and tasks.

## Classes

`class ` [bytearray](todo) | 
An alternative to std::vector<uint8_t>     
`class ` [Object](todo) | 
Base class for all reference-counted types.     
`class ` [sp< T >](todo) | 
A smart pointer class that holds a strong reference to an Object-derived type.     
`class ` [string](todo) | 
An alternative to `stdstring`.     
`class ` [Task](todo) | 
Any cancelable operation that performs a callback on the main thread when finished, unless the operation was cancelled.     
`class ` [Timer](todo) | 
A simple timer class.     
