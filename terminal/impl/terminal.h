#ifndef TBOX_TELNETD_TERMINAL_IMPL_H_20220128
#define TBOX_TELNETD_TERMINAL_IMPL_H_20220128

#include "../terminal.h"
#include <tbox/base/cabinet.hpp>

namespace tbox::terminal {

class SessionImpl;

class Terminal::Impl {
  public:
    ~Impl();

  public:
    SessionToken newSession(Connection *wp_conn);
    bool deleteSession(const SessionToken &session);
    bool input(const SessionToken &session, const std::string &str);

  public:
    NodeToken create(const EndNode &info);
    NodeToken create(const DirNode &info);
    NodeToken root() const;
    NodeToken find(const std::string &path) const;
    bool mount(const NodeToken &parent, const NodeToken &child, const std::string &name);

  protected:
    cabinet::Cabinet<SessionImpl> sessions_;
};

}

#endif //TBOX_TELNETD_TERMINAL_IMPL_H_20220128
