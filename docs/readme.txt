
To update Oaknut's documentation website:

1. Run Doxygen to extract the reference documentation from the Oaknut
source code into an intermediate XML form

```
cd _doxygen
doxygen
cd ..
```

2. Convert this XML into the HTML reference documentation using a
custom Java utility named DoxyPP.

```
_doxypp/bin/doxypp -i _doxygen/xml -o ref
```

3. Commit changes to GitHub and GitHub pages should automatically
regenerate the website.
