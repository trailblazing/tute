#ifndef FOR_EACH_H
#define FOR_EACH_H

#include <utility>
#include <tuple>



#include <map>

#include "boost/mpl/at.hpp"
#include <boost/fusion/tuple.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/mpl/int.hpp>

// http://stackoverflow.com/questions/26606865/static-for-cycle

//struct dumy {};

template <size_t I, typename Functor, typename = std::make_index_sequence<I> >
struct Apply;

template <size_t I, typename Functor, std::size_t ... Indices>
struct Apply<I, Functor, std::index_sequence<Indices ...> >
	: private std::tuple<Functor> // For EBO with functors
{
	Apply(Functor f) : std::tuple<Functor>(f){}
	Apply() = default;

//	template <typename InputRange1, typename InputRange2, typename OutputRange>
//	void operator ()(OutputRange &dst, const InputRange1 &lhs, const InputRange2 &rhs) const {
//		(void) std::initializer_list<int>
//		{
//			(std::get<Indices>(dst) = std::get<0>(*this)(std::get<Indices>(lhs), std::get<Indices>(rhs)), 0) ...
//		};
//	}

	template <typename InputRange>
	void operator ()(const InputRange &&lhs) const {
		(void) std::initializer_list<int>
		{
//			(std::get<Indices>(dst) =
//			(std::get<0>(*this)(*boost::mpl::advance<boost::mpl::int_<Indices> >(lhs)), 0) ...
//			(std::get<0>(*this)(boost::fusion::get<Indices>(lhs)), 0) ...	// work
//			(std::get<0>(*this)(boost::fusion::at<boost::mpl::int_<Indices>>(lhs)), 0) ...	// work
			(std::get<0>(*this)(std::get<Indices>(std::forward<InputRange const>(lhs))), 0) ...
//			(std::get<0>(*this)(boost::fusion::get<Indices>(lhs)), 0) ...
		};
	}
};


#endif // FOR_EACH_H
