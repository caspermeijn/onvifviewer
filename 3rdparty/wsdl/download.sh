#!/bin/sh
rm -rf docs.oasis-open.org www.onvif.org www.w3.org
wget -r --accept wsdl,xsd -I specs,ver10,ver20 https://www.onvif.org/profiles/specifications/
wget -r http://www.w3.org/2005/05/xmlmime
wget -r http://www.w3.org/2003/05/soap-envelope
wget -r http://docs.oasis-open.org/wsn/b-2.xsd
wget -r http://www.w3.org/2004/08/xop/include
wget -r http://www.w3.org/2005/08/addressing/ws-addr.xsd
wget -r http://docs.oasis-open.org/wsn/t-1
wget -r http://docs.oasis-open.org/wsrf/bf-2.xsd
wget -r http://docs.oasis-open.org/wsn/t-1.xsd
wget -r http://www.w3.org/2001/xml.xsd
mv www.onvif.org/ver20/media/wsdl/media.wsdl www.onvif.org/ver20/media/wsdl/media2.wsdl
