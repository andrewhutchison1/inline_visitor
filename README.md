# inline_visitor

`inline_visitor` is a lightweight C++11 utility that localises the visitation
of `Boost.Variant` objects, leveraging the power of C++11's lambdas against
the robust machinery of `Boost.Variant`. Using this header, you can visit
`boost::variant` objects "inline" without having to use the verbose and
cumbersome `static_visitor` idiom explicitly.

### Installation

Simply copy the file `inline_visitor.hpp` into your project directory or
anywhere on your compiler's include path. A C++11 capable compiler is required.

### Usage

```
boost::variant<int, std::string> variant{"Hello"s};

// We can create the visitor and apply it at the same time:
auto result = inline_visitor<bool>(
	[](int i){ return true; },
	[](std::string const& s){ return false; }).apply(variant); // false

// Or, we can create the visitor and use it in the same manner one would use
// boost::static_visitor...
auto visitor = inline_visitor<bool>(
	[](int i){ return true; },
	[](std::string const& s){ return false; });

// ...but it is arguably less verbose
visitor.apply(variant); // false
variant = 0;
visitor.apply(variant); // true
```

### Author and acknowledgements

Andrew Hutchison (andrew@andrewhutchison.net). I was partly inspired to make
this after encountering [this question](stackoverflow.com/questions/7867555) on Stack Overflow.
