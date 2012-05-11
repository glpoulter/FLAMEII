/*!
 * \file src/exceptions/mem.hpp
 * \author Shawn Chin
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief Exceptions thrown by memory managers
 */
#ifndef EXCEPTIONS__MEM_HPP_
#define EXCEPTIONS__MEM_HPP_
#include "base.hpp"

namespace flame { namespace exceptions {

class flame_mem_exception : public flame_exception {
  public:
    explicit flame_mem_exception(const std::string& msg)
        : flame_exception(msg) {}
};

class invalid_type : public flame_mem_exception {
  public:
    explicit invalid_type(const std::string& msg)
        : flame_mem_exception(msg) {}
};

class invalid_agent : public flame_mem_exception {
  public:
    explicit invalid_agent(const std::string& msg)
        : flame_mem_exception(msg) {}
};

class invalid_variable : public flame_mem_exception {
  public:
    explicit invalid_variable(const std::string& msg)
        : flame_mem_exception(msg) {}
};


}}  // namespace flame::exceptions
#endif  // EXCEPTIONS__MEM_HPP_
