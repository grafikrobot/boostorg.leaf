// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#if defined(BOOST_LEAF_NO_EXCEPTIONS) || !BOOST_LEAF_CFG_CAPTURE

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
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/exception.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/on_error.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

template <class... E, class F>
void test( F f_ )
{
    auto f =
        [=]
        {
            return leaf::try_handle_some(
                [&]() -> leaf::result<void>
                {
                    f_();
                    BOOST_TEST(false);
                    return { };
                },
                []( leaf::dynamic_capture const & cap ) -> leaf::result<void>
                {
                    return cap;
                });
        };

    {
        int c=0;
        leaf::result<void> r = f();
        leaf::try_catch(
            [&r]
            {
                return r.value();
            },
            [&c]( info<1> const & x )
            {
                BOOST_TEST_EQ(x.value, 1);
                BOOST_TEST_EQ(c, 0);
                c = 1;
            },
            [&c]
            {
                BOOST_TEST_EQ(c, 0);
                c = 2;
            } );
        BOOST_TEST_EQ(c, 1);
    }

    {
        int c=0;
        leaf::result<void> r = f();
        leaf::try_catch(
            [&r]
            {
                return r.value();
            },
            [&c]( info<2> const & x )
            {
                BOOST_TEST_EQ(x.value, 2);
                BOOST_TEST_EQ(c, 0);
                c = 1;
            },
            [&c]
            {
                BOOST_TEST_EQ(c, 0);
                c = 2;
            } );
        BOOST_TEST_EQ(c, 2);
    }

    {
        leaf::result<void> r = f();
        int what = leaf::try_catch(
            [&r]
            {
                (void) r.value();
                return 0;
            },
            []( info<1> const & x )
            {
                BOOST_TEST_EQ(x.value, 1);
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(what, 1);
    }

    {
        leaf::result<void> r = f();
        int what = leaf::try_catch(
            [&r]
            {
                (void) r.value();
                return 0;
            },
            []( info<2> const & x )
            {
                BOOST_TEST_EQ(x.value, 2);
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(what, 2);
    }

    {
        leaf::result<void> r = f();
        bool what = leaf::try_catch(
            [&r]
            {
                (void) r.value();
                return true;
            },
            []( info<1> const &, info<2> const & )
            {
                return true;
            },
            []( info<1> const & x, info<3> const & y )
            {
                BOOST_TEST_EQ(x.value, 1);
                BOOST_TEST_EQ(y.value, 3);
                return false;
            },
            []( info<1> const & )
            {
                return true;
            },
            []
            {
                return true;
            } );
        BOOST_TEST(!what);
    }

    {
        leaf::result<void> r = f();
        bool what = leaf::try_catch(
            [&r]
            {
                (void) r.value();
                return false;
            },
            []( info<1> const &, info<2> const & )
            {
                return false;
            },
            []( info<1> const & x, info<3> const & y )
            {
                BOOST_TEST_EQ(x.value, 1);
                BOOST_TEST_EQ(y.value, 3);
                return true;
            },
            []( info<1> const & )
            {
                return false;
            },
            []
            {
                return false;
            } );
        BOOST_TEST(what);
    }
}

int main()
{
    test<info<1>, info<2>, info<3>>(
        []
        {
            leaf::throw_exception(info<1>{1}, info<3>{3}); // Derives from leaf::leaf::error_id
        } );
    test<info<1>, info<2>, info<3>>(
        []
        {
            auto load = leaf::on_error( info<1>{1}, info<3>{3} );
            leaf::throw_exception(); // Derives from leaf::leaf::error_id
        } );
    test<info<1>, info<2>, info<3>>(
        []
        {
            auto load = leaf::on_error( info<1>{1}, info<3>{3} );
            throw std::exception(); // Does not derive from leaf::leaf::error_id
        } );
    return boost::report_errors();
}

#endif
