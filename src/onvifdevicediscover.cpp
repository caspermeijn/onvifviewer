/* Copyright (C) 2019 Casper Meijn <casper@meijn.net>
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "onvifdevicediscover.h"

#ifdef WITH_KDSOAP_WSDISCOVERY_CLIENT
#include <KDSoapWSDiscoveryClient/WSDiscoveryClient>
#include <KDSoapWSDiscoveryClient/WSDiscoveryTargetService>
#include <KDSoapWSDiscoveryClient/WSDiscoveryProbeJob>
#include <QDebug>
#include <QSharedPointer>
#endif

OnvifDeviceDiscover::OnvifDeviceDiscover(QObject* parent) :
    QObject(parent)
{
    qRegisterMetaType<QObjectList> ("QObjectList");

#ifdef WITH_KDSOAP_WSDISCOVERY_CLIENT
    m_client = new WSDiscoveryClient(this);

    m_probeJob = new WSDiscoveryProbeJob(m_client);
    connect(m_probeJob, &WSDiscoveryProbeJob::matchReceived, this, &OnvifDeviceDiscover::matchReceived);
    KDQName type("tdn:NetworkVideoTransmitter");
    type.setNameSpace("http://www.onvif.org/ver10/network/wsdl");
    m_probeJob->addType(type);
#endif
}

bool OnvifDeviceDiscover::isAvailable()
{
#ifdef WITH_KDSOAP_WSDISCOVERY_CLIENT
    return true;
#else
    return false;
#endif
}

QObjectList OnvifDeviceDiscover::matchList() const
{
    QObjectList list;
    for (auto match : m_matchMap.values()) {
        list.append(match);
    }
    return list;
}

void OnvifDeviceDiscover::start()
{
#ifdef WITH_KDSOAP_WSDISCOVERY_CLIENT
    m_client->start();
    m_probeJob->start();
#endif
}

void OnvifDeviceDiscover::stop()
{
#ifdef WITH_KDSOAP_WSDISCOVERY_CLIENT
    m_probeJob->stop();
#endif
}

void OnvifDeviceDiscover::matchReceived(const QSharedPointer<WSDiscoveryTargetService>& matchedService)
{
#ifdef WITH_KDSOAP_WSDISCOVERY_CLIENT
    OnvifDeviceDiscoverMatch* deviceMatch = m_matchMap.value(matchedService->endpointReference());
    if (deviceMatch == nullptr) {
        deviceMatch = new OnvifDeviceDiscoverMatch();
    }
    deviceMatch->m_endpoint = matchedService->endpointReference();
    for (auto& scope : matchedService->scopeList()) {
        if (scope.scheme() == "onvif" &&
                scope.authority().toLower() == "www.onvif.org") {
            auto splitPath = scope.path().split("/", QString::SkipEmptyParts);
            if (splitPath[0].toLower() == "name") {
                deviceMatch->m_name = splitPath[1];
            }
            if (splitPath[0].toLower() == "hardware") {
                deviceMatch->m_hardware = splitPath[1];
            }
        }
    }
    deviceMatch->m_xAddr = matchedService->xAddrList();
    deviceMatch->m_host.clear();
    for (auto& xAddr : matchedService->xAddrList()) {
        deviceMatch->m_host << xAddr.authority();
    }
    deviceMatch->m_lastSeen = matchedService->lastSeen();

    m_matchMap.insert(deviceMatch->m_endpoint, deviceMatch);
    emit matchListChanged(matchList());
#endif
}

QString OnvifDeviceDiscoverMatch::getHardware() const
{
    return m_hardware;
}

QString OnvifDeviceDiscoverMatch::getEndpoint() const
{
    return m_endpoint;
}

QList<QUrl> OnvifDeviceDiscoverMatch::getXAddr() const
{
    return m_xAddr;
}

QStringList OnvifDeviceDiscoverMatch::getHost() const
{
    return m_host;
}

QString OnvifDeviceDiscoverMatch::getName() const
{
    return m_name;
}
