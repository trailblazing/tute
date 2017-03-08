

#include "variant.h"
#ifdef TEST
int test_variant() {
	typedef fusion::result_of::as_vector<
	    fusion::joint_view<mpl::vector<bool, std::string>, A>>::type types1;
	typedef boost::make_variant_over<types1>::type variant1;

	variant1 a = 3.5f;
	variant1 b = true;

	std::cout << a << "\n " << b << ".\n There are "
	          << fusion::result_of::size<variant1::types>::type::value
	          << " elements in the variant" << std::endl;
	fusion::for_each(variant1::types(), printer());
	std::cout << std::endl;

	typedef mpl::fold<A, mpl::set<std::string, int>,
	                  mpl::insert<mpl::_1, mpl::_2>>::type types2;
	typedef boost::make_variant_over<
	    fusion::result_of::as_vector<types2>::type>::type variant2;

	variant2 a2 = 4;
	variant2 b2 = "bla";

	std::cout << a2 << "\n " << b2 << ".\n There are "
	          << fusion::result_of::size<variant2::types>::type::value
	          << " elements in the variant" << std::endl;
	fusion::for_each(variant2::types(), printer());
	std::cout << std::endl;
	return 0;
}

int test_unify() {
	typedef boost::mpl::insert<
	    boost::mpl::insert<boost::mpl::insert<boost::mpl::set<>, int>::type,
	                       float>::type,
	    int *>::type Set;

	typedef Unify<std::tuple, Set, boost::mpl::size<Set>::type::value>::type Set2;
	Set2 set_2;
	//	//This compile error will print the type of Set2
	//	Set2::asdfl;
	return 0;
}

#endif  // TEST
