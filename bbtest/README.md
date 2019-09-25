# Black-box tests

This directory contains black-box tests.

## A note about \_\_soll.xml-files

Files that end in `__soll.xml` do not actually contain valid XML: They just
contain a subset of the lines that is expected to be the output.

## How test cases are selected

Test cases are selected to represent important aspects of the ANL such as for
example carrier sensing or collisions.

## Running

Python3 is required. Running the tests in this directory requires the built
shared object to be present (see README.md in the parent directory!).
Requirements from the parent README.md are also valid here.

The tests are built and ran using `make debug`.
