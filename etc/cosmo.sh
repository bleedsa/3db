#!/bin/sh

R="`pwd`"

mkdir -p ape
curl --output ape/cosmocc.zip "https://cosmo.zip/pub/cosmocc/cosmocc.zip"

cd ape; {
	unzip cosmocc.zip
}; cd "$R"
