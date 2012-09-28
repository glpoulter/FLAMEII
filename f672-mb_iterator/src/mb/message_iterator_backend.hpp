/*!
 * \file FILENAME
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief DESCRIPTION
 */
#ifndef MB__MESSAGE_ITERATOR_BACKEND_HPP_
#define MB__MESSAGE_ITERATOR_BACKEND_HPP_
#include "boost/shared_ptr.hpp"
#include "mb_common.hpp"

namespace flame { namespace mb {

//! Abstract base class for an iterator backend
class MessageIteratorBackend
{
  public:
    typedef boost::shared_ptr<MessageIteratorBackend> Handle;

    virtual ~MessageIteratorBackend() {}
    virtual bool AtEnd(void) const = 0;
    virtual void Next(void) = 0;
    virtual void Rewind(void) = 0;
    virtual MessageHandle GetMessage(void) = 0;

    /*!
     * \brief Determine if a this backend is mutable
     * \return true/false
     *
     * Immutable backends access raw pointers directly and is therefore fast.
     *
     * Mutable version access via a index array and is slower compared to
     * immutable ones. However, the list of indices can be modified and so
     * the iterator is randomisable, sortable, and can be used to iterate a
     * through a non-contiguous subset of messages
     */
    virtual bool IsMutable(void) = 0;

    /*!
     * \brief Return a mutable backend with the same content
     * \return Handle to new backend
     *
     * For an immutable backend, this will generate and return a mutable
     * counterpart.
     *
     * If called on a mutable backend, a handle to the same
     */
    virtual Handle GetMutableVersion(void) = 0;

    //! \todo (lsc) Set-like operations to form new iterators for existing ones
};

}}  // namespace flame::mb
#endif  // MB__MESSAGE_ITERATOR_BACKEND_HPP_
