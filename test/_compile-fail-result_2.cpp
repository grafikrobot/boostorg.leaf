// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.
// Thanks @strager

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf.hpp>
#include <string>

struct A
{
    explicit A(int) { };
};

boost::leaf::result<A> f()
{
    return 42;
}
