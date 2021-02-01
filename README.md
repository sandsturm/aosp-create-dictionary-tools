# aosp-create-dictionary-tools

Basic workflow of the program is:
1. Read spellcheck file(s)
This is done to cross-check the incoming words from the new source file.
2. Read existing Android dictionary
This is done to balance the input and for completness.
3. Read custom spellcheck file.
Control which words should be added in any case.
4. Export new dictionary file.
5. Export file with words missing in the spellcheck file(s).
