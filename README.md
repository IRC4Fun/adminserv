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

OperServ SPECS
--------------

To update OperServ to add privilege information from AdminServ, you must patch
OperServ to include the privileges. This patch is `operserv/specs.patch` and
can be applied as follows:

````
cd atheme/modules/operserv
patch specs.c ../contrib/adminserv/operserv/specs.patch
make specs.so
make install
````

Don't forget to reload the module in atheme afterwards.
