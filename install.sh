#!/bin/bash

# Run as sudo. ie: sudo ./install.sh

cd "`dirname \"$0\"`"

install cryptkeeper /usr/bin/
install cryptkeeper_password /usr/bin/

echo "Cryptkeeper installed!"
