// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from varnames.djinni

#include "_varname_interface_.hpp"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class DBVarnameInterface;

namespace djinni_generated {

class VarnameInterface
{
public:
    using CppType = std::shared_ptr<::testsuite::VarnameInterface>;
    using CppOptType = std::shared_ptr<::testsuite::VarnameInterface>;
    using ObjcType = DBVarnameInterface*;

    using Boxed = VarnameInterface;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

