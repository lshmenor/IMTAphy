/**********************************************

License: BSD
Project Webpage: http://cajun-jsonapi.sourceforge.net/
Author: Terry Caton

***********************************************/

#ifndef WNS_PROBE_BUS_JSON_VISITOR_HPP
#define WNS_PROBE_BUS_JSON_VISITOR_HPP

#include <WNS/probe/bus/json/elements.hpp>

namespace wns { namespace probe  { namespace bus { namespace json {


class Visitor
{
public:
   virtual ~Visitor() {}

   virtual void Visit(Array& array) = 0;
   virtual void Visit(Object& object) = 0;
   virtual void Visit(Number& number) = 0;
   virtual void Visit(String& string) = 0;
   virtual void Visit(Boolean& boolean) = 0;
   virtual void Visit(Null& null) = 0;
};

class ConstVisitor
{
public:
   virtual ~ConstVisitor() {}

   virtual void Visit(const Array& array) = 0;
   virtual void Visit(const Object& object) = 0;
   virtual void Visit(const Number& number) = 0;
   virtual void Visit(const String& string) = 0;
   virtual void Visit(const Boolean& boolean) = 0;
   virtual void Visit(const Null& null) = 0;
};

} // json
} // bus
} // probe
} // wns

#endif // WNS_PROBE_BUS_JSON_VISITOR_HPP
