/*!
 * \file src/compat/C/message_iterator_wrapper.hpp
 * \author Shawn Chin
 * \date October 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Wrapper for MessageIterator to allow type-free Posting of messages
 */
#ifndef COMPAT__C__MESSAGE_ITERATOR_WRAPPER_HPP_
#define COMPAT__C__MESSAGE_ITERATOR_WRAPPER_HPP_
#include "mb/message_iterator.hpp"
#include "flame_compat_c.hpp"

namespace flame { namespace compat { namespace c {

class MessageIteratorWrapper {
  public:
    static const std::string MESSAGE_VARNAME;

    explicit MessageIteratorWrapper(flame::mb::MessageIteratorHandle h)
        : handle_(h) {}
    virtual ~MessageIteratorWrapper() {}
    virtual void GetMessage(void *out_ptr) = 0;
    bool AtEnd(void) const { return handle_->AtEnd(); }
    bool Next(void) { return handle_->Next(); }

  protected:
    flame::mb::MessageIteratorHandle handle_;
};

template <typename T>
class MessageIteratorWrapperImpl : public MessageIteratorWrapper {
  public:
    explicit MessageIteratorWrapperImpl(flame::mb::MessageIteratorHandle h)
        : MessageIteratorWrapper(h) {}  // call parent constructor

    void GetMessage(void *out_ptr) {
      T* typed_out_ptr = static_cast<T*>(out_ptr);
      *typed_out_ptr = handle_->Get<T>(Constants::MESSAGE_VARNAME);
    }
};
}}}  // namespace flame::compat::c
#endif  // COMPAT__C__MESSAGE_ITERATOR_WRAPPER_HPP_