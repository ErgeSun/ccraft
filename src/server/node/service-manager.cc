/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <algorithm>

#include "../../common/common-def.h"
#include "elector-manager-service.h"
#include "client-rpc-service.h"
#include "raft-consensus.h"
#include "raft-state-machine.h"

#include "service-manager.h"

namespace ccraft {
namespace server {
void ServiceManager::ServiceBootstrap::Run() {
    if (!s_bServsBooted) {
        ccsys::SpinLock l(&s_boot_mtx);
        if (!s_bServsBooted) {
            create_services();
            if (!start_services()) {
                throw std::runtime_error("Cannot start services!");
            }
            s_bServsBooted = true;
        }
    }
}

void ServiceManager::ServiceBootstrap::create_services() {
    for (auto st : SERVICES_TYPES) {
        switch (st) {
            case ServiceType::ElectorManager: {
                ServiceManager::s_mapServices[st] = new ElectorManagerService();
            }
            case ServiceType::ClientRpc: {
                ServiceManager::s_mapServices[st] = new ClientRpcService();
                break;
            }
            case ServiceType::RaftConsensus: {
                ServiceManager::s_mapServices[st] = new RaftConsensus();
                break;
            }
            case ServiceType ::RaftStateMachine: {
                ServiceManager::s_mapServices[st] = new RfStateMachine();
                break;
            }
        }
    }
}

bool ServiceManager::ServiceBootstrap::start_services() {
    for (auto s : ServiceManager::s_mapServices) {
        if (!s.second->Start()) {
            LOGEFUN << "Cannot start service " << (int32_t)s.first;
            return false;
        }
    }

    return true;
}

void ServiceManager::ServiceDestroyer::Run() {
    std::unique_lock<std::mutex> l(s_mtxServsLiving);
        std::for_each(ServiceManager::s_mapServices.begin(),
                      ServiceManager::s_mapServices.end(),
                      [](std::pair<ServiceType, IService*> p) {
                          p.second->Stop();
                      });

    for (auto s : ServiceManager::s_mapServices) {
        if (!s.second->Stop()) {
            LOGEFUN << "Cannot stop service " << (int32_t)s.first;
        }
    }

    s_bAllServsStopped = true;
    s_cvServsLiving.notify_one();
}

IService* ServiceManager::GetService(ServiceType st) {
    if (s_mapServices.end() == s_mapServices.find(st)) {
        return nullptr;
    }
    return s_mapServices[st];
}

void ServiceManager::ServiceDestroyer::WaitAllServicesStopped() {
    std::unique_lock<std::mutex> l(s_mtxServsLiving);
    while (!s_bAllServsStopped) {
        s_cvServsLiving.wait(l);
    }
}

std::map<ServiceType, IService*> ServiceManager::s_mapServices = std::map<ServiceType, IService*>{};
ccsys::spin_lock_t ServiceManager::ServiceBootstrap::s_boot_mtx = UNLOCKED;
bool ServiceManager::ServiceBootstrap::s_bServsBooted = false;
std::mutex ServiceManager::ServiceDestroyer::s_mtxServsLiving;
bool ServiceManager::ServiceDestroyer::s_bAllServsStopped = false;
std::condition_variable ServiceManager::ServiceDestroyer::s_cvServsLiving;
}
}
