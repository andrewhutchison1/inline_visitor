#ifndef INLINE_VISITOR_HPP_
#define INLINE_VISITOR_HPP_

#include <functional>

#include <boost/variant.hpp>

namespace detail {

template <typename Ret, typename... Fs>
struct generate_visitor;

template <typename Ret, typename F, typename... Fs>
struct generate_visitor<Ret, F, Fs...>
		: public generate_visitor<Ret, Fs...>
		, private F
{
	using F::operator();
	using generate_visitor<Ret, Fs...>::operator();

	generate_visitor(F f, Fs... fs)
		: generate_visitor<Ret, Fs...>{fs...}
		, F{f}
	{}
};

template <typename Ret, typename F>
struct generate_visitor<Ret, F>
		: public generate_visitor<Ret>
		, private F
{
	using F::operator();

	generate_visitor(F f)
		: generate_visitor<Ret>{}, F{f}
	{}
};

template <typename Ret>
struct generate_visitor<Ret>
		: public boost::static_visitor<Ret>
{
	generate_visitor()
	{}
};

template <typename Ret, typename Visitor>
struct inline_visitor_impl
{
	inline_visitor_impl(Visitor visitor)
		: visitor_{visitor}
	{}

	template <typename Variant>
	Ret apply(Variant&& variant)
	{
		return boost::apply_visitor(visitor_, std::forward<Variant>(variant));
	}
private:
	Visitor visitor_;
};

} // namespace detail

template <typename Ret, typename... Fs>
auto inline_visitor(Fs... fs)
		-> detail::inline_visitor_impl<
				Ret, detail::generate_visitor<Ret, Fs...>>
{
	return detail::inline_visitor_impl<
			Ret,
			detail::generate_visitor<Ret, Fs...>>{
					detail::generate_visitor<Ret, Fs...>{fs...}};
}

#endif // INLINE_VISITOR_HPP_
