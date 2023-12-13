// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#if !BOOST_LEAF_CFG_CAPTURE

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#else

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/handle_errors.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct err
{
    static int count;

    err()
    {
        ++count;
    }

    err( err const & )
    {
        ++count;
    }

    err( err && )
    {
        ++count;
    }

    ~err()
    {
        --count;
    }
};
template <int N>
int err<N>::count = 0;

int main()
{
    {
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( err<1>, err<2> &, leaf::dynamic_capture const & cap )
            {
                BOOST_TEST_EQ(err<1>::count, 2);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 0);
                BOOST_TEST(cap.empty());
            },
            []
            {
            });
        BOOST_TEST_EQ(err<1>::count, 0);
        BOOST_TEST_EQ(err<2>::count, 0);
    }

    {
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( leaf::dynamic_capture const & cap )
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 2);
                BOOST_TEST(!cap.empty());
            } );
        BOOST_TEST_EQ(err<1>::count, 0);
        BOOST_TEST_EQ(err<2>::count, 0);
    }

    {
        leaf::result<void> r = leaf::try_handle_some(
            []() -> leaf::result<void>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( leaf::dynamic_capture const & cap ) -> leaf::result<void>
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 2);
                BOOST_TEST(!cap.empty());
                return cap;
            } );
        BOOST_TEST_EQ(err<1>::count, 1);
        BOOST_TEST_EQ(err<2>::count, 1);
        (void) r;
    }
    BOOST_TEST_EQ(err<1>::count, 0);
    BOOST_TEST_EQ(err<2>::count, 0);

    {
        leaf::result<int> r = leaf::try_handle_some(
            []() -> leaf::result<int>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( leaf::dynamic_capture const & cap ) -> leaf::result<int>
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 2);
                BOOST_TEST(!cap.empty());
                return cap;
            } );
        BOOST_TEST_EQ(err<1>::count, 1);
        BOOST_TEST_EQ(err<2>::count, 1);
        (void) r;
    }
    BOOST_TEST_EQ(err<1>::count, 0);
    BOOST_TEST_EQ(err<2>::count, 0);

    return boost::report_errors();
}

#endif
