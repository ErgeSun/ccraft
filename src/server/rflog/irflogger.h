/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_SERVER_RFLOG_IRFLOGGER_H
#define CCRAFT_SERVER_RFLOG_IRFLOGGER_H

#include <google/protobuf/repeated_field.h>

using namespace google::protobuf;

namespace ccraft {
namespace protocol {
    class RfLogEntry;
}

using protocol::RfLogEntry;
namespace rflog {
class IRfLogger {
public:
    virtual ~IRfLogger() = default;
    /**
     *
     * @return 你无须释放。
     */
    virtual const RfLogEntry* GetLastEntry() = 0;
    /**
     *
     * @return 你无须释放。
     */
    virtual const RfLogEntry* GetEntry(uint32_t index) = 0;
    virtual void AppendEntries(RepeatedPtrField<RfLogEntry>* entries) = 0;
    virtual void AppendEntry(RfLogEntry* entry) = 0;
    virtual void Sync() = 0;
};
} // namespace rflog
} // namespace ccraft

#endif //CCRAFT_SERVER_RFLOG_IRFLOGGER_H
