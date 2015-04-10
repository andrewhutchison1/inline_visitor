#ifndef INLINE_VISITOR_HPP_
#define INLINE_VISITOR_HPP_

#include <functional>

#include <boost/variant.hpp>

namespace detail {

// This template 'generates' a structure inheriting ultimately
// from boost::static_visitor (transitively) through each recursive
// inheritance of itself
template <typename Ret, typename... Fs>
struct generate_visitor;

// Initial and recursive case. This case handles the first N-1 lambdas,
// with each instantiation 'implementing' a single lambda operator()
template <typename Ret, typename F, typename... Fs>
struct generate_visitor<Ret, F, Fs...>
		: public generate_visitor<Ret, Fs...>
		, private F
{
	using F::operator();

	// Bring the next instantiation's operator() into scope
	// which is the operator() of the next lambda
	using generate_visitor<Ret, Fs...>::operator();

	generate_visitor(F f, Fs... fs)
		: generate_visitor<Ret, Fs...>{fs...}
		, F{f}
	{}
};

// Penultimate case. We need to make this case explicit because
// we can't bring generate_visitor<Ret>::operator() into scope
// because (among other things) it doesn't actually exist. This case
// handles the Nth (final) lambda.
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

// Terminal case. We inherit from static_visitor here so boost can
// use our lambdas as if it were a visitor. The resulting class contains
// all overloads of operator() provided by the lambdas at the top level
template <typename Ret>
struct generate_visitor<Ret>
		: public boost::static_visitor<Ret>
{
	generate_visitor()
	{}
};

// This template ties together the logic in generate_visitor with
// the user-facing inline_visitor convenience function
template <typename Ret, typename Visitor>
struct inline_visitor_impl
{
	inline_visitor_impl(Visitor visitor)
		: visitor_{visitor}
	{}

	// As in boost::apply_visitor
	template <typename Variant>
	Ret apply(Variant&& variant)
	{
		return boost::apply_visitor(visitor_, std::forward<Variant>(variant));
	}
private:
	Visitor visitor_;
};

} // namespace detail

// inline_visitor essentially splits the logic of boost::apply_visitor into
// two: first, you call inline_visitor with lambdas. This returns an object
// that you may then (immediately or at a later stage) apply to a variant.
// This two-stage logic is convenient because it lets you define visitors
// more akin to a classical static_visitor in a less verbose manner. See
// README.md for more details and examples
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
