/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include "../../codegen/client-server.pb.h"

#include "raft-state-machine.h"

namespace ccraft {
    namespace server {
        RfStateMachine::RfStateMachine() {

        }

        RfStateMachine::~RfStateMachine() {

        }

        void RfStateMachine::Apply(std::vector<protocal::StateMachineCommand*> commands) {
            for (auto command : commands) {
                m_mapKVs[command->key()] = command->value();
            }
        }

        std::string RfStateMachine::GetValue(const std::string &key) {
            return this->m_mapKVs[key];
        }
    } // namespace server
} // namespace ccraft
