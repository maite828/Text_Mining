TID Language Detector
=====================

TID Language Detector is legacy code from 1991. 

It was an existing project in TID and has been extended to support Nutch2.

Building the applications
-------------------------
TID Language Detector is comprised of several C++ applications and det_id_nutch2 is one of them.
Since the binaries are platform dependent, you must compile the code as part of the installation in a new host. 

All you need to do is to execute the make command from the source directory of the application you want.
Then move the binary to the bin directory.

**Note**: The make command does not move the compiled file to the *bin* directory; this must be done manually.

Example (det_id_nutch2 application):

```
cd src/apps/det_id_nutch2
make
mv det_id_nutch2 ../../../bin/det_id_nutch2
```

det_id_nutch2
-------------

det_id_nutch takes as input a dump output file from Nutch2. 
It reads the file and when it finds the tag **text:start** it applies language detection to the text up to the **tag text:end**.
When detection is done it adds two new tags: **language_id** and **language_trust**

Notes:

1. If the text between text:start and text:end tags is empty, language detector ignores them and does not add language tags.
1. When a text can be equally assigned more than one language, language_trust is 0  
