adminserv
=========

A service for atheme offering "psuedo-oper" services.

Compatability
-------------

This has only been tested with Charybdis and relies on the TS6 protocol
(specifically for remote WHOWAS requests). Any server that allows remote WHOWAS
requests should be compatible, provided they use the same numerics. See
`whowas.c` for details about the numerics involved. If you are not running a
server that runs the TS6 protocol, do NOT load the whowas module.

Building & Installation
-----------------------

Clone the repository inside of atheme and run make:

````
cd atheme/modules/contrib
git clone https://github.com/brennie/adminserv.git
cd adminserv
make
make install
````
