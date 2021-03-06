/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_TOMSGPACKVISITOR_HPP
#define CLUON_TOMSGPACKVISITOR_HPP

#include "cluon/MsgPackConstants.hpp"
#include "cluon/cluon.hpp"

#include <cstdint>
#include <sstream>
#include <string>

namespace cluon {
/**
This class encodes a given message in MsgPack format.
*/
class LIBCLUON_API ToMsgPackVisitor {
   private:
    ToMsgPackVisitor(const ToMsgPackVisitor &) = delete;
    ToMsgPackVisitor(ToMsgPackVisitor &&)      = delete;
    ToMsgPackVisitor &operator=(const ToMsgPackVisitor &) = delete;
    ToMsgPackVisitor &operator=(ToMsgPackVisitor &&) = delete;

   public:
    ToMsgPackVisitor()  = default;
    ~ToMsgPackVisitor() = default;

    /**
     * @return Encoded data in MsgPack format.
     */
    std::string encodedData() const noexcept;

   public:
    // The following methods are provided to allow an instance of this class to
    // be used as visitor for an instance with the method signature void accept<T>(T&);

    void preVisit(int32_t id, const std::string &shortName, const std::string &longName) noexcept;
    void postVisit() noexcept;

    void visit(uint32_t id, std::string &&typeName, std::string &&name, bool &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, char &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint8_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint16_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint32_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, int64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, uint64_t &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, float &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, double &v) noexcept;
    void visit(uint32_t id, std::string &&typeName, std::string &&name, std::string &v) noexcept;

    template <typename T>
    void visit(uint32_t &id, std::string &&typeName, std::string &&name, T &value) noexcept {
        (void)id;
        (void)typeName;

        encode(m_buffer, name);
        {
            cluon::ToMsgPackVisitor nestedMsgPackEncoder;
            value.accept(nestedMsgPackEncoder);
            const std::string tmp{nestedMsgPackEncoder.encodedData()};
            const uint32_t LENGTH{static_cast<uint32_t>(tmp.size())};
            m_buffer.write(tmp.c_str(), static_cast<std::streamsize>(LENGTH));
        }
        m_numberOfFields++;
    }

   private:
    void encode(std::ostream &o, const std::string &s);
    void encodeUint(std::ostream &o, uint64_t v);
    void encodeInt(std::ostream &o, int64_t v);

   private:
    uint32_t m_numberOfFields{0};
    std::stringstream m_buffer{""};
};
} // namespace cluon

#endif
