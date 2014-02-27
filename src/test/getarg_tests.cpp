#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CTC");
    BOOST_CHECK(GetBoolArg("-CTC"));
    BOOST_CHECK(GetBoolArg("-CTC", false));
    BOOST_CHECK(GetBoolArg("-CTC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CTCo"));
    BOOST_CHECK(!GetBoolArg("-CTCo", false));
    BOOST_CHECK(GetBoolArg("-CTCo", true));

    ResetArgs("-CTC=0");
    BOOST_CHECK(!GetBoolArg("-CTC"));
    BOOST_CHECK(!GetBoolArg("-CTC", false));
    BOOST_CHECK(!GetBoolArg("-CTC", true));

    ResetArgs("-CTC=1");
    BOOST_CHECK(GetBoolArg("-CTC"));
    BOOST_CHECK(GetBoolArg("-CTC", false));
    BOOST_CHECK(GetBoolArg("-CTC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCTC");
    BOOST_CHECK(!GetBoolArg("-CTC"));
    BOOST_CHECK(!GetBoolArg("-CTC", false));
    BOOST_CHECK(!GetBoolArg("-CTC", true));

    ResetArgs("-noCTC=1");
    BOOST_CHECK(!GetBoolArg("-CTC"));
    BOOST_CHECK(!GetBoolArg("-CTC", false));
    BOOST_CHECK(!GetBoolArg("-CTC", true));

    ResetArgs("-CTC -noCTC");  // -CTC should win
    BOOST_CHECK(GetBoolArg("-CTC"));
    BOOST_CHECK(GetBoolArg("-CTC", false));
    BOOST_CHECK(GetBoolArg("-CTC", true));

    ResetArgs("-CTC=1 -noCTC=1");  // -CTC should win
    BOOST_CHECK(GetBoolArg("-CTC"));
    BOOST_CHECK(GetBoolArg("-CTC", false));
    BOOST_CHECK(GetBoolArg("-CTC", true));

    ResetArgs("-CTC=0 -noCTC=0");  // -CTC should win
    BOOST_CHECK(!GetBoolArg("-CTC"));
    BOOST_CHECK(!GetBoolArg("-CTC", false));
    BOOST_CHECK(!GetBoolArg("-CTC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CTC=1");
    BOOST_CHECK(GetBoolArg("-CTC"));
    BOOST_CHECK(GetBoolArg("-CTC", false));
    BOOST_CHECK(GetBoolArg("-CTC", true));

    ResetArgs("--noCTC=1");
    BOOST_CHECK(!GetBoolArg("-CTC"));
    BOOST_CHECK(!GetBoolArg("-CTC", false));
    BOOST_CHECK(!GetBoolArg("-CTC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CTC", "eleven"), "eleven");

    ResetArgs("-CTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CTC", "eleven"), "");

    ResetArgs("-CTC=");
    BOOST_CHECK_EQUAL(GetArg("-CTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CTC", "eleven"), "");

    ResetArgs("-CTC=11");
    BOOST_CHECK_EQUAL(GetArg("-CTC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CTC", "eleven"), "11");

    ResetArgs("-CTC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CTC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CTC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CTC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CTC", 0), 0);

    ResetArgs("-CTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-CTC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CTC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CTC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CTC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CTC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CTC");
    BOOST_CHECK_EQUAL(GetBoolArg("-CTC"), true);

    ResetArgs("--CTC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CTC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCTC");
    BOOST_CHECK(!GetBoolArg("-CTC"));
    BOOST_CHECK(!GetBoolArg("-CTC", true));
    BOOST_CHECK(!GetBoolArg("-CTC", false));

    ResetArgs("-noCTC=1");
    BOOST_CHECK(!GetBoolArg("-CTC"));
    BOOST_CHECK(!GetBoolArg("-CTC", true));
    BOOST_CHECK(!GetBoolArg("-CTC", false));

    ResetArgs("-noCTC=0");
    BOOST_CHECK(GetBoolArg("-CTC"));
    BOOST_CHECK(GetBoolArg("-CTC", true));
    BOOST_CHECK(GetBoolArg("-CTC", false));

    ResetArgs("-CTC --noCTC");
    BOOST_CHECK(GetBoolArg("-CTC"));

    ResetArgs("-noCTC -CTC"); // CTC always wins:
    BOOST_CHECK(GetBoolArg("-CTC"));
}

BOOST_AUTO_TEST_SUITE_END()
