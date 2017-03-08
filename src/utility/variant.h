#ifndef VARIANT_H
#define VARIANT_H

#include <iostream>
#include <string>

#include <boost/variant.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/joint_view.hpp>
#include <boost/fusion/include/mpl.hpp>  //important: allows compatibility fusion-mpl
#include <boost/fusion/include/set.hpp>

#include <boost/mpl/fold.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/vector.hpp>

namespace fusion = boost::fusion;
namespace mpl = boost::mpl;

struct A {
	int a1;
	double a2;
	float a3;
};

BOOST_FUSION_ADAPT_STRUCT(A, (int, a1)(double, a2)(float, a3))

struct printer {
	template <typename T>
	void operator()(T) const {
		std::cout << "typeid :" << typeid(T).name() << ";\n";
	}
};

// http://stackoverflow.com/questions/28585599/how-to-obtain-standard-mpl-sequence-after-fold

#include <boost/mpl/erase_key.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/size.hpp>
#include <tuple>

template <template <class...> class OutSeqType, class Sequence,
          std::size_t nSeqSize, class... Elements>
struct Unify {
	typedef typename boost::mpl::front<Sequence>::type Next;
	typedef typename Unify<OutSeqType,
	                       typename boost::mpl::erase_key<Sequence, Next>::type,
	                       nSeqSize - 1, Next, Elements...>::type type;
};

template <template <class...> class OutSeqType, class Sequence,
          class... Elements>
struct Unify<OutSeqType, Sequence, 0ul, Elements...> {
	typedef OutSeqType<Elements...> type;
};

#endif  // VARIANT_H
