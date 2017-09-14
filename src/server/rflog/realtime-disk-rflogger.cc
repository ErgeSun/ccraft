/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#include <memory>
#include <unistd.h>
#include <sys/mman.h>

#include "../../common/file-utils.h"
#include "../../common/io-utils.h"
#include "../../common/codec-utils.h"
#include "../../common/buffer.h"
#include "../../rpc/protobuf-utils.h"

#include "realtime-disk-rflogger.h"

#define WriteFileFully(fd, buf, size)                                                                       \
        if (-1 == common::IOUtils::WriteFully(fd, buf, size)) {                                             \
            auto err = errno;                                                                               \
            LOGFFUN << "write file " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);  \
        }

#define ReadFileFully(fd, buf, size)                                                                        \
        if (-1 == common::IOUtils::ReadFully_V2(fd, buf, size)) {                                           \
            auto err = errno;                                                                               \
            LOGFFUN << "read file " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);   \
        }

#define LSeekFile(fd, len, where)                                                                           \
        if (-1 == lseek(fd, len, where)) {                                                                  \
            auto err = errno;                                                                               \
            LOGFFUN << "lseek file " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);  \
        }

namespace ccraft {
    namespace rflog {
        RtDiskRfLogger::RtDiskRfLogger(std::string &&logFilePath) : m_sLogFilePath(std::move(logFilePath)) {
            initialize();
        }

        RtDiskRfLogger::~RtDiskRfLogger() {
            if (-1 != m_iFd) {
                close(m_iFd);
                m_iFd = -1;
            }
        }

        RfLogEntry *RtDiskRfLogger::GetLastEntry() {
            if (m_vEntries.empty()) {
                return nullptr;
            }

            return m_vEntries[m_vEntries.size()];
        }

        RfLogEntry *RtDiskRfLogger::GetEntry(uint32_t index) {
            return nullptr;
        }

        bool RtDiskRfLogger::AppendEntries(RepeatedPtrField<RfLogEntry> *entries) {
            return false;
        }

        bool RtDiskRfLogger::AppendEntry(RfLogEntry *entry) {
            return false;
        }

        void RtDiskRfLogger::initialize() {
            if (!common::FileUtils::Exist(m_sLogFilePath)) {
                LOGDFUN2("create rflog file ", m_sLogFilePath.c_str());
                if (-1 == (m_iFd = common::FileUtils::Open(m_sLogFilePath, O_WRONLY, O_CREAT, 0644))) {
                    auto err = errno;
                    LOGFFUN << "create rflog file " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);
                }

                // write file header.
                uchar header[RFLOG_MAGIC_NO_LEN];
                ByteOrderUtils::WriteUInt32(header, RFLOG_MAGIC_NO);
                WriteFileFully(m_iFd, header, RFLOG_MAGIC_NO_LEN);
            } else {
                if (-1 == (m_iFd = common::FileUtils::Open(m_sLogFilePath, O_RDWR, 0, 0))) {
                    auto err = errno;
                    LOGFFUN << "open rflog file " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);
                }

                // 1. check if file is empty.
                auto fileSize = common::FileUtils::GetFileSize(m_iFd);
                if (-1 == fileSize) {
                    auto err = errno;
                    LOGFFUN << "get file size for " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);
                }
                if (0 == fileSize) {
                    LOGDFUN3("rflog file ", m_sLogFilePath.c_str(), " is empty!");
                    return;
                }

                // 2. check if file header is corrupt.
                if (fileSize < RFLOG_MAGIC_NO_LEN) {
                    LOGFFUN << "rflog file " << m_sLogFilePath.c_str() << " header is corrupt!";
                }

                auto mapRet = mmap(nullptr, (size_t)fileSize, PROT_READ, MAP_PRIVATE, m_iFd, 0);
                if (MAP_FAILED == mapRet) {
                    auto err = errno;
                    LOGFFUN << "mmap rflog " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);
                }

                auto buf = (uchar*)mapRet;
                uint32_t offset = 0;
                uint32_t magicNo = ByteOrderUtils::ReadUInt32(buf);
                if (RFLOG_MAGIC_NO != magicNo) {
                    LOGFFUN << "rflog " << m_sLogFilePath.c_str() << " header is corrupt!";
                }

                offset += RFLOG_MAGIC_NO_LEN;
                common::Buffer b;
                // 3. load rflog content
                while (offset <= fileSize - 1) {
                    // 3.1 read rflog len
                    auto len = ByteOrderUtils::ReadUInt32(buf + offset);
                    auto startPos = ByteOrderUtils::ReadUInt32(buf + offset + len);
                    if (startPos != offset) {
                        LOGFFUN << "parse rflog " << m_sLogFilePath.c_str() << " failed at offset " << offset;
                    }

                    auto startPtr = buf + offset;
                    auto endPtr = buf + len - 1;
                    b.Refresh(startPtr, endPtr, startPtr, endPtr, nullptr);
                    auto rflogEntry = new RfLogEntry();
                    rpc::ProtoBufUtils::Deserialize(&b, rflogEntry);

                    m_vEntries.push_back(rflogEntry);
                    offset += (len + sizeof(uint32_t));
                }

                if (-1 == munmap(mapRet, fileSize)) {
                    auto err = errno;
                    LOGFFUN << "munmap file " << m_sLogFilePath.c_str() << " failed with errmsg " << strerror(err);
                }
            }
        }
    } // namespace rflog
} // namespace ccraft
