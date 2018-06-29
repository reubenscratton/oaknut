
Oaknut's documentation website is built with Jekyll and Doxygen. The
`jekyll` subfolder contains the Jekyll template site including
the generated reference documentation.

Doxygen is used to extract the reference documentation from the Oaknut
source code into an intermediate XML form, which is then converted by
a custom Java utility named DoxyPP.

The script to rebuild the documentation and site is:

```
cd doxygen
doxygen
cd ..
doxypp -i doxygen/xml -o jekyll/ref
```

You can then run jekyll or deploy the jekyll subfolder to Github Pages
