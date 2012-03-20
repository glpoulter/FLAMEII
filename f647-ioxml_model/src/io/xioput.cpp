/*!
 * \file src/io/xioput.cpp
 * \author Simon Coakley
 * \date 2012
 * \copyright Copyright (c) 2012 STFC Rutherford Appleton Laboratory
 * \copyright Copyright (c) 2012 University of Sheffield
 * \copyright GNU Lesser General Public License
 * \brief XIOput: holds input/output information
 */
#include <cstdio>
#include <string>
#include <vector>
#include "./xioput.hpp"

namespace flame { namespace io {

XIOput::XIOput() {
    /* Set default options */
    random_ = false;
}

XIOput::~XIOput() {
}

void XIOput::print() {
    std::fprintf(stdout, "\t\t\tMessage Name: %s\n", getMessageName().c_str());
    if (random_) std::fprintf(stdout, "\t\t\tRandom: true\n");
    if (sort_) {
        std::fprintf(stdout,
                "\t\t\tSort:\n\t\t\t\tKey: %s\n\t\t\t\tOrder: %s\n",
                getSortKey().c_str(),
                getSortOrder().c_str());
    }
}

void XIOput::setMessageName(std::string name) {
    messageName_ = name;
}

std::string XIOput::getMessageName() {
    return messageName_;
}

void XIOput::setRandom(bool random) {
    random_ = random;
}

bool XIOput::getRandom() {
    return random_;
}

void XIOput::setSort(bool sort) {
    sort_ = sort;
}

bool XIOput::getSort() {
    return sort_;
}

void XIOput::setSortKey(std::string key) {
    sortKey_ = key;
}

std::string XIOput::getSortKey() {
    return sortKey_;
}

void XIOput::setSortOrder(std::string order) {
    sortOrder_ = order;
}

std::string XIOput::getSortOrder() {
    return sortOrder_;
}

}}  // namespace flame::io