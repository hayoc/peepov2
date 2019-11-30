<img src="https://i.imgur.com/asqHaeo.png" alt="peepo" align="left" height="150" width="150"/>

## peepo: an attempt at artificial general intelligence

> [N]othing seems more possible to me than that people some day will come to the definite opinion that there is no copy in the [...] nervous system which corresponds to a particular thought, or a particular idea, or memory. (Ludwig Wittgenstein, Writings on the Philosophy of Psychology, 1948)

For more info [peepo.ai](https://peepo.ai) or follow our project board [taiga](https://tree.taiga.io/project/hayoc-peepo/backlog)

This project is the C++ port of the original prototype peepo, which was written in Python and can be found here: https://github.com/hayoc/peepo_prototype.

##### Setup

In Visual Studio 2019, add the following to 

project properties > configuration properties > C/C++ > Command Line > Additional Properties:
`/std:c++latest /permissive- /experimental:preprocessor /Zc:__cplusplus`