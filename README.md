# fastcgi++ lite #

**Original Author:** Eddie Carle

**Reduced and modified by:** Tobias Flaig

## About ##

This branch contains a reduced and slightly modified version of eddic/fastcgipp. 
All SQL, email, and session features have been removed, and compatibility has 
been improved (e.g., this variant will compile on Debian Jessie). In addition,
it contains slight modifications for use in [NAWA](https://github.com/jatofg/nawa).
Please note that C++17 is required for this variant.

If you just need the FastCGI functionality, this version will provide it 
without unnecessary bloat. The following description is taken from the 
upstream project (and modified accordingly):

This library is intended as a high-efficiency C++17 api for web development. It
allows your applications to communicate with web servers through the FastCGI
protocol, tabulates all your environment data, manages character encoding, and
allows requests to effectively share CPU time. If you want any further
information check the Doxygen documentation associated with the respective
release, or build it yourself. Be sure to read through the examples.

## Releases ##

There are no release files available for the lite variant. Just clone this 
repository and build it.

## Building ##

(Instructions taken from the upstream repository and adapted.)

This should provide you with all the basic stuff you need to do to get fastcgi++
built and installed. The build system is CMake and the following instructions
assume you are in Bash.

First we need to clone.

    git clone https://github.com/jatofg/fastcgipp.git fastcgi++

Then we make a build directory.

    mkdir fastcgi++.build
    cd fastcgi++.build

Now we need run cmake.

    cmake -DCMAKE_BUILD_TYPE=RELEASE ../fastcgi++

Note that that was to do a release build. That means heavily optimized and not
good for debugging. If you want to do some debugging to either fastcgi++ or an
application you are developing that uses fastcgi++, do a debug build.

    cmake -DCMAKE_BUILD_TYPE=DEBUG ../fastcgi++

Or if you want some really hardcore debug and diagnostics info

    cmake -DCMAKE_BUILD_TYPE=DEBUG -D LOG_LEVEL:INT=4 ../fastcgi++

Now let's build the library itself.

    make

Then we can build the documentation if we so desire.

    make fcgidoc

Now let's install it all (doc included if it was built).

    make install

Maybe we should build the unit tests?

    make fcgitests

And of course we should run them as well.

    make test

And hey, let's build the examples too!

    make fcgiexamples
