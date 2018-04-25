#!/bin/sh
rm -rf www.onvif.org
wget -r --accept wsdl,xsd -I specs,ver10,ver20 https://www.onvif.org/profiles/specifications/
