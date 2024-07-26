#include <string>
#include <iostream>
//#include <variant>
#include <mpark/variant.hpp>

struct SampleVisitor
{
    void operator()(int i) const { 
        std::cout << "int: " << i << "\n"; 
    }
    void operator()(float f) const { 
        std::cout << "float: " << f << "\n"; 
    }
    void operator()(const std::string& s) const { 
        std::cout << "string: " << s << "\n"; 
    }
};

int main()
{
    mpark::variant<int, float, std::string> intFloatString;
    static_assert(mpark::variant_size_v<decltype(intFloatString)> == 3);

    // default initialized to the first alternative, should be 0
    mpark::visit(SampleVisitor{}, intFloatString);

    // index will show the currently used 'type'
    std::cout << "index = " << intFloatString.index() << std::endl;
    intFloatString = 100.0f;
    std::cout << "index = " << intFloatString.index() << std::endl;
    intFloatString = "hello super world";
    std::cout << "index = " << intFloatString.index() << std::endl;

    // try with get_if:
    if (const auto intPtr (mpark::get_if<int>(&intFloatString)); intPtr) 
        std::cout << "int!" << *intPtr << "\n";
    else if (const auto floatPtr (mpark::get_if<float>(&intFloatString)); floatPtr) 
        std::cout << "float!" << *floatPtr << "\n";

    if (mpark::holds_alternative<int>(intFloatString))
        std::cout << "the variant holds an int!\n";
    else if (mpark::holds_alternative<float>(intFloatString))
        std::cout << "the variant holds a float\n";
    else if (mpark::holds_alternative<std::string>(intFloatString))
        std::cout << "the variant holds a string\n";  

    // try/catch and bad_variant_access
    try 
    {
        auto f = mpark::get<float>(intFloatString); 
        std::cout << "float! " << f << "\n";
    }
    catch (mpark::bad_variant_access&) 
    {
        std::cout << "our variant doesn't hold float at this moment...\n";
    }

    // visit:
    mpark::visit(SampleVisitor{}, intFloatString);
    intFloatString = 10;
    mpark::visit(SampleVisitor{}, intFloatString);
    intFloatString = 10.0f;
    mpark::visit(SampleVisitor{}, intFloatString);
}

