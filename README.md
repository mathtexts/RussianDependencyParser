RussianDependencyParser
=======================

Morphological disambiguation (C++) and dependency parser of Russian by Alexey Gorelov, supervised by Dr Mstislav Maslennikov

Nickname: ASynt 14

This is a project for the "Mathematical methods for text analysis" course in autumn 2013 in MSU. 

Kindly refer to:
- main1.pdf
- vk.com/mathtexts -> achievements of listeners (http://vk.com/topic-50553132_28815796)

Compulsory citation:
Alexey Gorelov and Mstislav Maslennikov, 2014. "Syntactic analysis of texts". In "Mathematical methods for text analysis" speckours, vk.com/mathtexts.


Installation
============

(1) Install TurboParser from the page http://www.ark.cs.cmu.edu/TurboParser/ into some directory,
    for example ~/install/TurboParser;

(2) Download the plain text dictionary from opencorpora.org/dict.php, eg into the file ../openCorpora/dict.opcorpora.txt;

(2) Update the file launch.sh with ~/install/TurboParser and ../openCorpora/ folder locations;

(3) Check some libraries, that are necessary to run TurboParser. You can copy them as follows:
    $ sudo cp ~/install/TurboParser/deps/local/lib/libgflags.so.2 /usr/lib/libgflags.so.2
    $ sudo cp ~/install/TurboParser/deps/local/lib/libglog.so.0 /usr/lib/libglog.so.0

(4) Setup your own configuration of launch.sh and run it. In the first run, you will need to (a) correct the
    dictionary; and (b) train the morphological model and syntactic parser. In the following runs, you can
    skip the preparation steps (a) and (b).


License
=======

   (C) Copyright 2014 Moscow State University.

   All rights reserved. This program and the accompanying materials
   are made available under the terms of the Eclipse Public License v1.0
   which accompanies this distribution, and is available at
   
   http://www.eclipse.org/legal/epl-v10.html
  
   Contributors:
       Alexey Gorelov, Elena Sokolova (dependency corpus extension), Mstislav Maslennikov
