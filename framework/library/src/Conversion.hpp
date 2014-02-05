#ifndef WNS_CONVERSION_HPP
#define WNS_CONVERSION_HPP

#include <string>
#include <sstream>
#include <boost/type_traits/is_pod.hpp>

namespace wns
{

    template <typename TO, typename FROM, bool POD>
    class Converter
    {
    public:
        TO
        operator ()(const FROM& from)
        {
            return from.template to<TO>();
        }
    };

    template<typename FROM>
    class Converter<std::string, FROM, true>
    {
    public:
        std::string
        operator ()(FROM from)
        {
             std::stringstream ss;
             ss << from;
             return ss.str();
        }
    };

    template<typename TO, typename FROM>
    TO
    to(const FROM& from)
    {
        Converter<TO, FROM, boost::is_pod<FROM>::value> converter;
        return converter(from);
    }
}

#endif // NOT defined WNS_CONVERSION_HPP
