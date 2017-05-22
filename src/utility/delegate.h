#ifndef DELEGATE_H_INCLUDED
#define DELEGATE_H_INCLUDED

#include <cstddef> // nullptr_t
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <tuple>
// #include <mpllibs/metaparse/string.hpp> // https://github.com/sabel83/mpllibs
#include <boost/mpl/string.hpp>
#include <limits> // error: ‘numeric_limits’ is not a member of ‘std’
#include <vector>

#include <memory>
#include <vector>
// #include <boost/archive/text_oarchive.hpp>            // for serialization
// #include <boost/archive/text_iarchive.hpp>            // for serialization
#include <boost/serialization/scoped_ptr.hpp> // for serialization
#include <boost/serialization/shared_ptr.hpp> // for serialization
#include <boost/serialization/tracking.hpp>   // for serialization
#include <boost/serialization/vector.hpp>     // for serialization
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <iostream>
#include <sstream>

// #include <remote/archive/text_iarchive.hpp>          // for serialization
// #include <remote/archive/text_oarchive.hpp>          // for serialization

// #include <remote/bindings/text_serializer.hpp>        // for serialization

#include <boost/pointer_cast.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>

using boost::shared_ptr;
using boost::weak_ptr;
using boost::make_shared;
using boost::static_pointer_cast;

#include <array>
#include <cstddef> // std::size_t
#include <float.h>
#include <limits>
#include <memory>
#include <string>

#undef max
// #pragma warning(disable : 4003)   // warning C4003 : not enough actual
// parameters for macro 'max'

#include <exception>
#include <iostream>
namespace sd {
	// http://stackoverflow.com/questions/20616958/stdforward-without-perfect-forwarding
	template <bool do_move, typename T>
	struct helper {
		auto operator()(T&& t) const -> decltype(std::move(t))
		{
			return std::move(t);
		}
	};

	template <typename T>
	struct helper<false, T> {
		T& operator()(T&& t) const
		{
			return t;
		}
	};

	template <bool do_move, typename T>
	auto conditional_move(T&& t)
	    -> decltype(helper<do_move, T>()(std::forward<T>(t)))
	{
		return helper<do_move, T>()(std::forward<T>(t));
	}

	template <typename A, typename B>
	struct equal_type {
		enum { value = false };
	};
	template <typename A>
	struct equal_type<A, A> {
		enum { value = true };
	};

	template <bool, typename T, typename F>
	struct static_if {
		typedef F type;
	}; // static const bool value = false;

	template <typename T, typename F>
	struct static_if<true, T, F> {
		typedef T type;
	}; // static const bool value = true;

	template <int factorial>
	struct _ {
		operator char()
		{
			return factorial + std::numeric_limits<int>::max();
		}
	}; // always overflow?

	template <typename T, typename shared_type>
	struct pointer_type_out;

	template <typename T>
	struct pointer_type_out<T, void*> {
		typedef typename static_if<std::is_pointer<T>::value, T, T*>::type type;
		typedef void* inner_type;
		typedef typename std::remove_pointer<T>::type raw_type;
	}; // object "this" pointer

	template <typename T>
	struct pointer_type_out<T, boost::shared_ptr<void>> {
		static_assert(std::is_pointer<T>::value == false, "You do not need a pointer since equiped with samrt pointer");
		typedef boost::shared_ptr<T> type;
		typedef boost::shared_ptr<void> inner_type;
		typedef typename std::remove_pointer<T>::type raw_type;
	};

	template <typename T>
	struct pointer_type_out<T, std::shared_ptr<void>> {
		static_assert(std::is_pointer<T>::value == false, "You do not need a pointer since equiped with samrt pointer");
		typedef std::shared_ptr<T> type;
		typedef std::shared_ptr<void> inner_type;
		typedef typename std::remove_pointer<T>::type raw_type;
	};

	//	template <typename T>
	//	struct pointer_type_out<boost::shared_ptr<T>, boost::shared_ptr<void>> {
	//		typedef boost::shared_ptr<T> type;
	//		typedef boost::shared_ptr<void> inner_type;
	//		typedef typename std::remove_pointer<T>::type raw_type;
	//	};
	//	template <typename T>
	//	struct pointer_type_out<std::shared_ptr<T>, std::shared_ptr<void>> {
	//		typedef std::shared_ptr<T> type;
	//		typedef std::shared_ptr<void> inner_type;
	//		typedef typename std::remove_pointer<T>::type raw_type;
	//	};
	// template<typename T>
	// struct static_if_shared <T, boost::intrusive_ptr<void>> {
	// typedef boost::intrusive_ptr<T> type;
	// };

	// template<typename T>
	// struct static_if_shared <T, boost::intrusive_ptr<void *>> {
	// typedef boost::intrusive_ptr<T> type;
	// };
	template <typename T, typename shared_type>
	typename pointer_type_out<T, shared_type>::inner_type
	pointer_in(shared_type obj);

	template <typename T, typename shared_type>
	typename pointer_type_out<T, shared_type>::type
	pointer_out(shared_type obj);

	template <typename T>
	typename pointer_type_out<T, void>::type
	pointer_out(void* obj)
	{
		typedef typename pointer_type_out<T, void>::type target_pointer_type;
		return static_cast<target_pointer_type>(obj);
	} // <T, void *>

	template <typename T>
	typename pointer_type_out<T, boost::shared_ptr<void>>::type
	pointer_out(boost::shared_ptr<void> obj)
	{
		typedef typename static_if<std::is_pointer<T>::value, typename std::remove_pointer<T>::type, T>::type target_raw_type;
		return boost::static_pointer_cast<target_raw_type>(obj);
	} // <T, boost::shared_ptr<void>>

	template <typename T>
	typename pointer_type_out<T, std::shared_ptr<void>>::type
	pointer_out(std::shared_ptr<void> obj)
	{
		typedef typename static_if<std::is_pointer<T>::value, typename std::remove_pointer<T>::type, T>::type target_raw_type;
		return std::static_pointer_cast<target_raw_type>(obj);
	} // <T, std::shared_ptr<void>>

	// you need boost::intrusive_ref_counter<typename, boost::thread_safe_counter>
	// act as the target class rather than void*
	// template<typename T>
	// typename static_if_shared<T, boost::intrusive_ptr<void>>::type
	// &static_if_shared_pointer_cast(boost::intrusive_ptr<void> &obj)    // <T,
	// boost::shared_ptr<void>>
	// {
	// return boost::static_pointer_cast<T>(obj);  //
	// boost::intrusive_ptr<T>(static_cast<T *>(obj.get()));    //
	// static_cast<typename static_if<std::is_pointer<T>::value, T, T *>::type>(obj)
	// }

	// template<typename T>
	// typename static_if_shared<T, boost::intrusive_ptr<void*>>::type
	// &static_if_shared_pointer_cast(boost::intrusive_ptr<void*> &obj)    // <T,
	// boost::shared_ptr<void>>
	// {
	// return boost::static_pointer_cast<T>(obj);  //
	// boost::intrusive_ptr<T>(static_cast<T *>(obj.get()));    //
	// static_cast<typename static_if<std::is_pointer<T>::value, T, T *>::type>(obj)
	// }

	// template<typename bailee, typename bailee_return, typename bailee_require>
	// struct _invoke_functor;

	// template<typename caller_require, typename bailee, typename bailee_return,
	// typename bailee_require>
	// struct _invoke_functor_return;

	// template<typename caller_require, typename bailee, typename bailee_return>
	// struct _invoke_functor;

	// template<typename bailee, typename bailee_return, typename bailee_require>
	// void _transmit(void *_caller_require, void *const _obj, void *const
	// _invoke_obj, void const *_bailee_requiree)
	// {
	// static_cast<bailee_return *>(_caller_require) = ((static_cast<bailee
	// *const>(_obj))->*((static_cast<_invoke_functor<bailee, void, bailee_require>
	// * const>(_invoke_obj))->inv))(*(static_cast<bailee_require const
	// *>(_bailee_requiree)));
	// }

	// template<typename bailee, typename bailee_require>
	// void _release(void *_invoke_functee)
	// {
	// delete static_cast<_invoke_functor<bailee, void, bailee_require>
	// *>(_invoke_functee);
	// }

	// class Agency {
	// protected:
	// void *const obj;
	// void (*inv)(void *_caller_require, void *const _obj, void *const _invoke_obj,
	// void const *_bailee_requiree);
	// void (*del)(void *);

	// void *invoke_functee;
	// void *bailee_requiree;
	// void *caller_requiree;

	// template<typename bailee, typename bailee_return, typename bailee_require>
	// struct _invoke_functor {
	// bailee_return (bailee::*inv)(bailee_require);
	// void (*void_pr)(void *const _obj, void const *_bailee_requiree);
	// _invoke_functor(bailee_return(bailee::*_inv)(bailee_require))
	// {
	// inv = _inv;
	// }

	// bailee_return excute(void *const _obj, void const *_bailee_requiree)
	// {
	// return ((static_cast<bailee *const>(_obj))->*inv)(static_cast<bailee_require
	// *const>(_bailee_requiree));
	// }

	////        template<typename bailee, typename bailee_return, typename
	///bailee_require>
	// static bailee_return _transmit(void *_caller_require, void *const _obj, void
	// *const _invoke_obj, void const *_bailee_requiree)
	// {
	// return *static_cast<bailee_return *>(_caller_require) = ((static_cast<bailee
	// *const>(_obj))->*((static_cast<_invoke_functor<bailee, bailee_return,
	// bailee_require> * const>(_invoke_obj))->inv))(*(static_cast<bailee_require
	// const *>(_bailee_requiree)));

	// }

	////        template<typename bailee, typename bailee_return, typename
	///bailee_require>
	// static void _release(void *_invoke_functee)
	// {
	// delete static_cast<_invoke_functor<bailee, bailee_return, bailee_require>
	// *>(_invoke_functee);
	// }

	// };

	// public:

	// template<typename bailee, typename bailee_return, typename bailee_require>
	// // _Convention see msdn: Argument Passing and Naming Conventions
	// Agency(bailee_return &caller_require, bailee &_obj,
	// bailee_return(bailee::*_inv)(bailee_require), bailee_require &input_pamater)
	// : obj(&_obj)
	// , invoke_functee((void *) new _invoke_functor<bailee, bailee_return,
	// bailee_require>(_inv)) // , invoke_functee((void *)0)
	// , inv(_invoke_functor<bailee, bailee_return, bailee_require>::_transmit)
	// , del(_invoke_functor<bailee, bailee_return, bailee_require>::_release)
	// , bailee_requiree((void *) &input_pamater)
	// , caller_requiree((void *) &caller_require)
	// {
	// }

	// void operator()()           // he can return bailee_require !!!, if
	// bailee_require is as same as object type? what will happen?
	// {
	// inv(caller_requiree, obj, invoke_functee, bailee_requiree);
	// }

	// ~ Agency()
	// {
	// if(nullptr != invoke_functee) del(invoke_functee);
	// }

	// };

	// template<typename caller_require>
	// class AgencyReturn {

	// protected:

	// caller_require (*inv)(void *const _obj, void *const _invoke_obj, void
	// *_bailee_requiree);
	// void (*del)(void *);
	// void *const obj;
	// void *invoke_functee;

	// template<typename caller_require_, typename bailee, typename bailee_return,
	// typename bailee_require>
	// struct _invoke_functor;

	// template<typename bailee, typename bailee_return, typename bailee_require>
	// struct _invoke_functor<caller_require, bailee, bailee_return, bailee_require>
	// {

	// caller_require (bailee::*inv)(bailee_require);

	// _invoke_functor(caller_require(bailee::*_inv)(bailee_require))
	// {
	// inv = _inv;
	// }

	////        template<typename caller_require, typename bailee, typename
	///bailee_return, typename bailee_require>
	// static caller_require _transmit_return(void *const _obj, void *const
	// _invoke_obj, void *_bailee_requiree)
	// {
	// return static_cast<caller_require>(((static_cast<bailee
	// *const>(_obj))->*((static_cast<_invoke_functor<caller_require, bailee,
	// bailee_return, bailee_require> *
	// const>(_invoke_obj))->inv))(*(static_cast<bailee_require
	// *>(_bailee_requiree))));

	// }

	////    template<typename caller_require, typename bailee, typename
	///bailee_return, typename bailee_require>
	// static void _release_return(void *_invoke_functee)
	// {
	// delete static_cast<_invoke_functor<caller_require, bailee, bailee_return,
	// bailee_require> *>(_invoke_functee);
	// }

	// };

	// public:

	// template<typename bailee, typename bailee_return, typename bailee_require>
	// // _Convention see msdn: Argument Passing and Naming Conventions
	// AgencyReturn(bailee &_obj, bailee_return(bailee::*_inv)(bailee_require)
	// //AgencyReturn(bailee & _obj, bailee_return(bailee::*_inv)(bailee_require) ,
	// bailee_require & input_pamater)
	// ) :
	// obj(&_obj)
	// , invoke_functee((void *) new _invoke_functor<caller_require, bailee,
	// bailee_return, bailee_require>(_inv))
	// , del(_invoke_functor<caller_require, bailee, bailee_return,
	// bailee_require>::_release_return)
	// , inv(_invoke_functor<caller_require, bailee, bailee_return,
	// bailee_require>::_transmit_return)
	// {
	// }

	// template<typename bailee_require>
	// caller_require operator()(bailee_require &input_pamater)
	// {
	//// he can return bailee_require !!!, if bailee_require is as same as object
	///type? what happen?
	// return inv(obj, invoke_functee, &input_pamater);
	// }

	// ~ AgencyReturn()
	// {
	// if(nullptr != invoke_functee)
	// del(invoke_functee);
	// }

	// };

	// template<typename caller_require>
	// struct AgencyRead {
	// protected:

	// caller_require (*inv)(void *const _obj, void *_invoke_obj);
	// void (*del)(void *);
	// void *const obj;
	// void *invoke_functee;
	// template<typename caller_require_, typename bailee, typename bailee_return>
	// struct _invoke_functor;

	// template<typename bailee, typename bailee_return>
	// struct _invoke_functor <caller_require, bailee, bailee_return>{

	// caller_require (bailee::*inv)();

	// _invoke_functor(caller_require(bailee::*_inv)())
	// {
	// inv = _inv;
	// }

	// static caller_require _transmit_return_read(void *const _obj, void
	// *_invoke_obj)
	// {
	// return static_cast<caller_require>(
	// (
	// (static_cast<bailee *const>(_obj))->*(
	// (
	// static_cast<_invoke_functor<caller_require, bailee, bailee_return>
	// *>(_invoke_obj)
	// )->inv
	// )
	// )
	// ()
	// );

	// }

	// static void _release_return_read(void *_invoke_functee)
	// {
	// delete static_cast<_invoke_functor<caller_require, bailee, bailee_return>
	// *>(_invoke_functee);
	// }
	// };
	// public:

	// template<typename bailee, typename bailee_return>
	// // _Convention see msdn: Argument Passing and Naming Conventions
	// AgencyRead(bailee &_obj, bailee_return(bailee::*_inv)()
	// ) : obj(&_obj)
	// , invoke_functee((void *) new _invoke_functor<caller_require, bailee,
	// bailee_return>(_inv))
	// , del(_invoke_functor<caller_require, bailee,
	// bailee_return>::_release_return_read)
	// , inv(_invoke_functor<caller_require, bailee,
	// bailee_return>::_transmit_return_read)
	// {
	// }

	// caller_require operator()()
	// // It can return bailee_require !!!, if bailee_require is as same as object
	// type? what happen?
	// {
	// return inv(obj, invoke_functee);
	// }

	// ~AgencyRead()
	// {
	// if(nullptr != invoke_functee) del(invoke_functee);
	// }
	// };

	// int test() {
	// char(_<Factorial<5>::value>()); //
	// http://stackoverflow.com/questions/4977715/calculating-and-printing-factorial-at-compile-time-in-c
	// return 0;
	// }

	template <int x>
	struct __;
	// moved to equal_type
	//	template <typename A, typename B>
	//	struct static_assert {
	//		static const bool value = false;
	//	};

	//	template <typename A>
	//	struct static_assert<A, A> {
	//		static const bool value = true;
	//	};

	template <std::size_t sa, std::size_t sb>
	struct equal_size {
		static const bool value = false;
	};

	template <std::size_t sz>
	struct equal_size<sz, sz> {
		static const bool value = true;
	};

	// python zip alternative
	template <class ContainerA>
	unsigned common_length(unsigned len, const ContainerA& first)
	{
		unsigned firstLen = first.size();
		if (len > firstLen)
			len = firstLen;
		return len;
	}
	template <class ContainerA, class... Containers>
	unsigned common_length(unsigned len, const ContainerA& first, const Containers&... rest)
	{
		unsigned firstLen = first.size();
		if (len > firstLen)
			len = firstLen;
		return common_length(len, rest...);
	}
	template <template <typename...> class Container, typename TypeA>
	std::tuple<TypeA> get_tuple_from(unsigned index, Container<TypeA> const& first)
	{
		return std::tuple<TypeA>(first[index]);
	}
	template <template <typename...> class Container, typename TypeA,
	    typename... Types>
	std::tuple<TypeA, Types...> get_tuple_from(unsigned index, Container<TypeA> const& first, Container<Types> const&... rest)
	{
		return std::tuple_cat(std::tuple<TypeA>(first[index]), get_tuple_from<Container, Types...>(index, rest...));
	}
	template <template <typename...> class Container, typename... Types>
	Container<std::tuple<Types...>> zip(Container<Types> const&... args)
	{
		unsigned len = common_length(std::numeric_limits<unsigned>::max(), args...);
		Container<std::tuple<Types...>> res;
		std::tuple<Types...> item;
		for (unsigned i = 0; i < len; i++) {
			item = get_tuple_from<Container, Types...>(i, args...);
			res.push_back(item);
		}
		return res;
	}
	// struct some_name {};

	// using aliases for cleaner syntax
	template <class T>
	using invoke = typename T::type;

	template <std::size_t...>
	struct sequence {
		using type = sequence;
	};

	template <class S1, class S2>
	struct concat;

	template <std::size_t... I1, std::size_t... I2>
	struct concat<sequence<I1...>, sequence<I2...>>
	    : sequence<I1..., (sizeof...(I1) + I2)...> {
	};

	template <class S1, class S2>
	using concat_invoke = invoke<concat<S1, S2>>;

	template <std::size_t N>
	struct generate_sequence;
	template <std::size_t N>
	using generate_sequence_invoke = invoke<generate_sequence<N>>;

	template <std::size_t N>
	struct generate_sequence : concat_invoke<generate_sequence_invoke<N / 2>, generate_sequence_invoke<N - N / 2>> {
	};

	template <>
	struct generate_sequence<0> : public sequence<> {
	};
	template <>
	struct generate_sequence<1> : public sequence<0> {
	};

	// template<> struct generate_sequence<1024*1024>{};

	template <typename bound>
	struct method;

	template <typename object_type, typename... Args>
	std::unique_ptr<object_type> make_unique(Args&&... args)
	{
		return std::unique_ptr<object_type>(new object_type(std::forward<Args>(args)...));
	}

	template <typename T>
	struct meta_info;

	template <size_t sz>
	struct size_type_value_trait {
		static const size_t value = sz;
	};

	template <typename _object_type, typename... _Arg>
	struct lazy_functor {
		// for serialize
		typedef _object_type object_type;
		typedef std::tuple<_Arg...> Arg;

		void (object_type::*_func)(_Arg...);

		lazy_functor(void (object_type::*func)(_Arg...))
		    : _func(func)
		{
		}

		static void transmit(std::shared_ptr<void> _functor, boost::shared_ptr<void> _object, _Arg&&... _arg)
		{
			auto obj = pointer_out<object_type>(_object);
			auto real_functor = pointer_out<lazy_functor>(_functor);

			(*obj.*real_functor->_func)(std::forward<_Arg>(_arg)...);
		}

	    private:
		lazy_functor(const lazy_functor& fun);
	};

	template <size_t I, typename T>
	struct remove_ith_type {
	};

	template <typename T, typename... Ts>
	struct remove_ith_type<0, std::tuple<T, Ts...>> {
		typedef std::tuple<Ts...> type;
	};

	template <size_t I, typename T, typename... Ts>
	struct remove_ith_type<I, std::tuple<T, Ts...>> {
		typedef decltype(tuple_cat(std::declval<std::tuple<T>>(), std::declval<typename remove_ith_type<I - 1, std::tuple<Ts...>>::type>()))
		    type;
	};

	template <typename T>
	struct remove_first_type {
		typedef std::tuple<T> type;
	};

	template <typename T, typename... Ts>
	struct remove_first_type<std::tuple<T, Ts...>> {
		typedef std::tuple<Ts...> type;
	};

	template <typename... Arg>
	struct sizeof_variadic {
		static const size_t value =
		    1 + sizeof_variadic<
			    typename remove_first_type<std::tuple<Arg...>>::type>::value;
	};

	template <typename Arg>
	struct sizeof_variadic<Arg> {
		static const size_t value = 1;
	}; // <Arg, Arg>

	template <typename object_type>
	static std::string data_transmit(void* t)
	{
		std::ostringstream os;
		os << *static_cast<object_type*>(t);
		return os.str();
	}
	template <typename object_type>
	struct type_router {
		object_type* t;
		type_router(object_type* t)
		    : t(t)
		{
		}
	};

	template <typename object_type>
	void delete_type_route(void* f)
	{
		if (f != nullptr) //(static_cast<type_router<object_type>*>(f) != (type_router<object_type>*)nullptr)
			delete static_cast<type_router<object_type>*>(f);
	}
	struct data;

	// template<typename object_type_>
	// inline void save_transmit(remote::archive::text_oarchive ar, const
	// shared_ptr<void> _obj, const unsigned int file_version)
	// {
	// ::boost::serialization::save_construct_data
	// //<remote::archive::text_oarchive, object_type_>
	// (ar, static_if_shared_pointer_cast<object_type_>(_obj), file_version);
	// }

	// template<typename object_type_>
	// inline void load_transmit(remote::archive::text_iarchive ar, shared_ptr<void>
	// _obj, const unsigned int file_version)
	// {
	// ::boost::serialization::load_construct_data
	// //<remote::archive::text_iarchive, object_type_>
	// (ar, static_if_shared_pointer_cast<object_type_>(_obj), file_version);
	// }

	template <typename>
	struct inner_type;

	template <typename T>
	struct inner_type {
		typedef void* type;
		typedef T real_type;
	};
	template <typename T>
	struct inner_type<std::shared_ptr<T>> {
		typedef std::shared_ptr<void> type;
		typedef T real_type;
	};
	template <typename T>
	struct inner_type<boost::shared_ptr<T>> {
		typedef boost::shared_ptr<void> type;
		typedef T real_type;
	};

	template <typename inner_pointer_type_>
	struct meta_info {
		typedef typename inner_type<inner_pointer_type_>::type inner_pointer_type;

	    protected:
		inner_pointer_type _object;

		std::string _object_type;
		std::string _object_name;

		// friend ::boost::serialization::access;
		// template<class Archive>
		// void serialize(Archive & ar, const unsigned int version)
		// {
		//// ** note that this is empty **
		// }
	    public:
		// void(*_save_transmit)(remote::archive::text_oarchive ar, const
		// shared_ptr<void> _obj, const unsigned int file_version);
		// void(*_load_transmit)(remote::archive::text_iarchive ar, shared_ptr<void>
		// _obj, const unsigned int file_version);

		template <typename object_pointer_type>
		meta_info(object_pointer_type o, std::string object_name = "")
		    : _object(static_pointer_cast<void>(o))
		    , _object_type(typeid(object_pointer_type).name())
		    , _object_name(object_name)
		{
			//			static_assert(std::is_pointer<target_type>::value == false, "meta_info only accept a pointer type");
		}

		//		template <typename object_type_>
		//		meta_info(object_type_* o, std::string object_name = "")
		//		    : _object(static_pointer_cast<typename inner_type<object_type_>::type>(o))
		//		    , _object_type(typeid(object_type_).name())
		//		    , _object_name(object_name)
		//		{
		//			//			static_assert(std::is_pointer<target_type>::value == false, "meta_info only accept a pointer type");
		//		}

		const std::string object_type() const
		{
			return _object_type;
		}

		const std::string object_name() const
		{
			return _object_name;
		}
		inner_pointer_type object()
		{
			return _object;
		}
		inner_pointer_type object() const
		{
			return _object;
		}
	};

	//// template<typename _meta_info>
	// struct _serialize // deprecated, because of moving from dlib to
	// boost::serialization
	// : public meta_info<shared_ptr<void>>
	// {

	// typedef meta_info<shared_ptr<void>> meta_info;
	// typedef method<meta_info> method;

	// void(*_save)(shared_ptr<void> functor_, shared_ptr<void> object_,
	// std::ostream& out);
	// void(*_load)(shared_ptr<void> functor_, shared_ptr<void> object_,
	// std::istream& in);

	// shared_ptr<void> _functor_save;
	// shared_ptr<void> _functor_load;

	// template <typename _object_type>
	// _serialize(shared_ptr<_object_type> o
	// , void(_object_type::*save)(std::ostream&)
	// , void(_object_type::*load)(std::istream&)
	// , std::string object_name = ""
	// ) :
	// meta_info(o, object_name)
	// ,
	// _functor_save(static_if_shared_pointer_cast<void>(boost::make_shared<lazy_functor<_object_type,
	// std::ostream&> >(save)))
	// ,
	// _functor_load(static_if_shared_pointer_cast<void>(boost::make_shared<lazy_functor<_object_type,
	// std::istream&> >(load)))
	// , _save(lazy_functor<_object_type, std::ostream&>::transmit)
	// , _load(lazy_functor<_object_type, std::istream&>::transmit)
	// {
	// }
	// void save(std::ostream& out) const
	// {
	// _save(_functor_save, _object, out);
	// }
	// void load(std::istream& in)
	// {
	// _load(_functor_load, _object, in);
	// }
	// };

	// wrap a trivial data type, or a serialize-able composite object?
	struct data {
		// useless, never get out, atomic data
		void* _value; // ?   // do not need name, name further in map<string,
			      // common_data>
		// void * _type_router;
		std::string _data_type;
		std::string _data_name; // not true, it need a name, and a unique name here,
					// others are references of this one

		// void (* _delete_type_route)(void *);
		std::string (*_funct)(void*);

		const std::string data_name() const
		{
			return _data_name;
		}

		const std::string data_type() const
		{
			return _data_type;
		}

		template <typename object_type> // for local using, not a distributed one
		data(object_type* value, std::string data_name = "")
		    : _value(value)
		    , _data_type(typeid(typename std::remove_reference<typename std::remove_pointer<decltype(value)>::type>::type).name())
		    , _data_name(data_name)
		    , _funct(data_transmit<object_type>)
		{
		}

		data& operator=(const data&) = delete;
		// {
		// this->_value = m._value;
		// this->_type_router  = m._type_router;   // need deep copy?
		// this->_name  = m._name;
		// this->_funct  = m._funct;
		// this->_type_name  = m._type_name;
		// this->_delete_type_route    = m._delete_type_route;
		//
		// return *this;
		// }

		std::string operator()()
		{
			return _funct(_value);
		} // not good, maybe lost precision

		~data()
		{
		} // _delete_type_route(_type_router);
	};

	// struct bound{};
	// struct unbound{};

	template <typename _meta_info = meta_info<void>> // = meta_info<boost::shared_ptr<void>>
	struct method;

	template <typename _meta_info> // meta_info<void *>
	struct method : public _meta_info {
		typedef _meta_info meta_info;
		typedef typename _meta_info::inner_pointer_type inner_pointer_type;

	    protected:
		// std::string _method_name;
		// std::string _return_type_name;    //, typeid(typename
		// std::remove_reference<return_type>::type).name()    // always can get,
		// nerver output

		std::shared_ptr<void> _closure;

		void (*_transmit)(std::shared_ptr<void> transmitter_, void* return_, inner_pointer_type object_);

		void* _rt = nullptr;

		std::mutex _mutex;

		// const size_t _parameter_num;
		// const char* arguments_types;
		// friend ::boost::serialization::access;
		// template<class Archive>
		// void serialize(Archive & ar, const unsigned int version)
		// {
		//// ** note that this is empty **
		// }

		template <typename _inner_pointer_type, typename _return_type, typename _object_type, typename... _Arg>
		struct closure_t {
		    public:
			typedef _inner_pointer_type inner_pointer_type;
			typedef _return_type return_type;
			typedef _object_type object_type;
			//			typedef std::tuple<_Arg...> Arg;
		    private:
			closure_t(const closure_t& fun);
			closure_t& operator=(const closure_t& fun);
			closure_t& operator=(closure_t&& fun);
			struct inner_impl {
				template <typename return_type, typename object_type, std::size_t... _I, typename... arg_t> //
				inner_impl(
				    return_type* _return,
				    typename pointer_type_out<object_type, inner_pointer_type>::type object,
				    std::shared_ptr<closure_t<inner_pointer_type, return_type, object_type, arg_t...>> real_functor,
				    sequence<_I...>)
				{
					try {
						*_return = (object->*(real_functor->_func))(std::get<_I>(*(real_functor->_arg))...);
					} catch (std::exception& e) {
						std::cerr << std::endl
							  << "Error : signature : " << typeid(real_functor->_func).name() << " name : " << real_functor->_method_name << " encountered : " << e.what();
					}
				}
				template <typename object_type, std::size_t... _I, typename... arg_t> //
				inner_impl(void* _return,
				    typename pointer_type_out<object_type, inner_pointer_type>::type object,
				    std::shared_ptr<closure_t<inner_pointer_type, void, object_type, arg_t...>> real_functor,
				    sequence<_I...>)
				{
					(void)_return;
					try {
						(object->*(real_functor->_func))(std::get<_I>(*(real_functor->_arg))...);
					} catch (std::exception& e) {
						std::cerr << std::endl
							  << "Error : signature : " << typeid(real_functor->_func).name() << " name : " << real_functor->_method_name << " encountered : " << e.what();
					}
				}
			};

		    protected:
			std::string _method_name;
			// std::string _return_type_name;
			// static constexpr size_t parameter_number = { sizeof...(_Arg) };
			return_type (object_type::*_func)(_Arg...);

			std::shared_ptr<std::tuple<_Arg...>> _arg;

		    public:
			closure_t(std::string _method_name, // , std::string _return_type_name
			    return_type (object_type::*func)(_Arg...), _Arg&&... arg)
			    : _method_name(_method_name) // , _return_type_name(typeid(typename std::remove_reference<return_type>::type).name())
			    , _func(func)
			    , _arg(std::make_shared<std::tuple<_Arg...>>(std::forward<_Arg>(arg)...))
			{
			}

			static void transmit(std::shared_ptr<void> _functor, void* _return, inner_pointer_type _object)
			{
				// auto obj = static_pointer_cast<object_type>(_object);
				// auto real_functor = std::static_pointer_cast<transmitter>(_functor);
				// auto return_ = static_cast<typename static_if<std::is_void<return_type>::value, void*, return_type*>::type>(_return);

				// constexpr auto return_type_is_void = is_void<return_type>::value;
				// constexpr auto parameter_number_is_zero = (parameter_number == 0);// (sizeof...(_Arg) == (size_t)0

				// typedef typename static_if < parameter_number_is_zero, struct inner_impl<object_type, void, 0, _Arg...>, struct inner_impl<object_type, void, parameter_number, _Arg...> >::type void_return;

				// typedef typename static_if < parameter_number_is_zero ), struct inner_impl<object_type, return_type, 0, _Arg...>, struct inner_impl<object_type, return_type, parameter_number, _Arg...> > ::type type_return;

				inner_impl(
				    static_cast<typename static_if<std::is_void<return_type>::value, void*, return_type*>::type>(_return),
				    static_pointer_cast<object_type>(_object),
				    std::static_pointer_cast<closure_t>(_functor),
				    generate_sequence<sizeof...(_Arg)>());
			}
		};

	    public:
		// std::string method_name()
		// {
		// return _method_name;
		// }
		// std::string return_type_name()
		// {
		// return _return_type_name;
		// }
		template <typename object_type>
		using raw_type = typename pointer_type_out<object_type, inner_pointer_type>::raw_type;

		template <typename return_type, typename object_type, typename... Arg>
		using closure_type = closure_t<inner_pointer_type, return_type, raw_type<object_type>, typename std::remove_reference<Arg>::type...>;

		template <typename return_type, typename object_type, typename... Arg>
		method(std::string _method_name,
		    return_type (object_type::*f)(Arg...),
		    return_type* r,                                                     // shared_ptr<object_type> o // = (object_type *const)0 //nullptr
		    typename pointer_type_out<object_type, inner_pointer_type>::type o, //typename pointer_type<object_type, pointer_type>::type o,
		    Arg... arg                                                          // use std::forward<Type> to supply
		    )
		    : meta_info(o, "")                                                                                                                                                                             // arguments_types(typeid(std::tuple<Arg ...>).name()) //(typeid(std::tuple<std::nullptr_t>).name())// , _method_name(_method_name), _return_type_name(typeid(typename std::remove_reference<return_type>::type).name()) // r != (return_type *)0? typeid(typename std::remove_reference<return_type>::type).name() : "void"
		    , _closure(std::static_pointer_cast<void>(std::make_shared<closure_type<return_type, object_type, Arg...>>(_method_name, f, std::forward<typename std::remove_reference<Arg>::type>(arg)...))) // , typeid(typename std::remove_reference<return_type>::type).name()
		    , _transmit(closure_type<return_type, object_type, Arg...>::transmit)                                                                                                                          //closure_t<inner_pointer_type, object_type, return_type, typename std::remove_reference<Arg>::type...>::operator()
		    , _rt(r)
		{
			static_assert(equal_type<object_type, raw_type<object_type>>::value, "contructing closure of different types");
			assert(o);
		}

		template <typename return_type, typename object_type, typename... Arg>
		method(std::string _method_name,
		    return_type (object_type::*f)(Arg...),
		    typename pointer_type_out<object_type, inner_pointer_type>::type o, //typename pointer_type<object_type, pointer_type>::type o,
		    Arg... arg                                                          // use std::forward<Type> to supply
		    )
		    : meta_info(o, "")                                                                                                                                                                      // arguments_types(typeid(std::tuple<Arg ...>).name()) //(typeid(std::tuple<std::nullptr_t>).name())// , _method_name(_method_name), _return_type_name(typeid(typename std::remove_reference<return_type>::type).name()) // r != (return_type *)0? typeid(typename std::remove_reference<return_type>::type).name() : "void"
		    , _closure(std::static_pointer_cast<void>(std::make_shared<closure_type<void, object_type, Arg...>>(_method_name, f, std::forward<typename std::remove_reference<Arg>::type>(arg)...))) // , typeid(typename std::remove_reference<return_type>::type).name()
		    , _transmit(closure_type<void, object_type, Arg...>::transmit)
		//		    , _rt(nullptr)
		{
			static_assert(equal_type<object_type, raw_type<object_type>>::value, "contructing closure of different types");
			assert(o);
		}

		method& operator=(const method&) = delete;

		void operator()() const
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_transmit(_closure, _rt, meta_info::_object);
		} // std::ostream& out

		void operator()()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_transmit(_closure, _rt, meta_info::_object);
		}

		// auto test() -> decltype(_transmit(0, 0, 0))   //, _parameter_num
		// {
		// return _transmit(_transmitter, _rt, meta_info::_object);
		//// return 0;
		// }
	};

	template <typename _meta_info, typename return_type, typename... Arg>
	struct _interface; // = meta_info

	// template <
	// typename _meta_info     // = meta_info
	// , typename return_type
	// , typename ... Arg
	// > struct _interface<_meta_info, return_type(Arg ...)> : public _meta_info {
	// typedef _meta_info meta_info;
	// typedef typename _meta_info::object_pointer_type object_pointer_type;
	// protected:
	// std::string _method_name;
	// std::string _return_type_name;

	// boost::shared_ptr<void> _transmitter;
	// return_type (*_transmit)(boost::shared_ptr<void> transmitter_,
	// object_pointer_type object_, Arg ...);

	// public:
	// std::string method_name()
	// {
	// return _method_name;
	// }
	// std::string return_type_name()
	// {
	// return _return_type_name;
	// }

	// template <typename object_type>
	// _interface(
	// std::string _method_name
	// , return_type(object_type::*f)(Arg ...)
	// , typename static_if_shared<object_type, object_pointer_type>::type o
	// //shared_ptr<object_type> o // = (object_type *const)0  //nullptr
	// ) : meta_info(o, "")
	// , _method_name(_method_name)
	// , _return_type_name(typeid(typename
	// std::remove_reference<return_type>::type).name())
	// ,
	// _transmitter(static_if_shared_pointer_cast<void>(boost::make_shared<transmitter_interface<object_pointer_type,
	// object_type, return_type, Arg ...> >(f)))
	// , _transmit(transmitter_interface<object_pointer_type, object_type,
	// return_type, Arg ...>::transmit)
	// {
	// }

	// _interface &operator = (const _interface &) = delete;     // because of deep
	// copy

	// auto operator()(Arg ... arg)->return_type {             // const // for
	// serialize()
	// return _transmit(_transmitter, meta_info::_object, std::forward<Arg>(arg)
	// ...);
	// }
	// };

	template <typename _meta_info, typename return_type,
	    typename... Arg> // = meta_info
	struct _interface<_meta_info, return_type(Arg...)> : public _meta_info {
		typedef _meta_info meta_info;
		typedef typename _meta_info::inner_pointer_type inner_pointer_type;

	    protected:
		std::string _method_name;
		std::string _return_type_name;

		boost::shared_ptr<void> _closure;
		return_type (*_transmit)(boost::shared_ptr<void> transmitter_, inner_pointer_type object_, Arg&&...);

		template <typename _object_type, typename _object_pointer_type = inner_pointer_type> // typename _object_pointer_type, typename// _object_type, typename _return_type,// typename ... _Arg
		struct closure_t {
			typedef _object_pointer_type object_pointer_type;
			// typedef _return_type return_type;
			typedef _object_type object_type;

			return_type (object_type::*_func)(Arg...);

			closure_t(return_type (object_type::*func)(Arg...))
			    : _func(func)
			{
			}

			static return_type transmit(boost::shared_ptr<void> _functor, object_pointer_type _object, Arg&&... arg)
			{
				return (*pointer_out<object_type>(_object).*pointer_out<closure_t>(_functor)->_func)(std::forward<Arg>(arg)...);
			}

		    private:
			closure_t(const closure_t&);
		};

		template <typename _object_pointer_type>
		struct closure_t<std::nullptr_t, _object_pointer_type> {
			// typedef _return_type return_type;
			typedef _object_pointer_type object_pointer_type;
			return_type (*_func)(Arg...);

			closure_t(return_type (*func)(Arg...))
			    : _func(func)
			{
			}

			static return_type transmit(boost::shared_ptr<void> _functor, object_pointer_type _object, Arg&&... arg)
			{
				(void)_object;
				return (pointer_out<closure_t>(_functor)->_func)(
				    std::forward<Arg>(arg)...);
			}

		    private:
			closure_t(const closure_t&);
		};

	    public:
		std::string method_name()
		{
			return _method_name;
		}
		std::string return_type_name()
		{
			return _return_type_name;
		}

		template <typename object_type>
		_interface(
		    return_type (object_type::*f)(Arg...),
		    typename pointer_type_out<object_type, inner_pointer_type>::type o,
		    std::string _method_name = "")
		    : meta_info(o, "")
		    , _method_name(_method_name)
		    , _return_type_name(typeid(typename std::remove_reference<return_type>::type).name())
		    , _closure(pointer_out<void>(boost::make_shared<closure_t<object_type, inner_pointer_type>>(f)))
		    , _transmit(closure_t<object_type, inner_pointer_type>::transmit)
		{
			if (!o)
				throw std::runtime_error("nullptr error");
		}
		_interface(return_type (*f)(Arg...), std::string _method_name = "")
		    : meta_info(inner_pointer_type(nullptr), "")
		    , _method_name(_method_name)
		    , _return_type_name(
			  typeid(typename std::remove_reference<return_type>::type).name())
		    , _closure(pointer_out<void>(boost::make_shared<closure_t<std::nullptr_t, inner_pointer_type>>(f)))
		    , _transmit(closure_t<std::nullptr_t, inner_pointer_type>::transmit)
		{
		}

		_interface& operator=(const _interface&) = delete; // because of deep copy

		auto operator()(Arg&&... arg) -> return_type
		{
			return _transmit(_closure, meta_info::_object, std::forward<Arg>(arg)...);
		} // const // for serialize()
	};

	// template <typename _meta_info, typename return_type, typename ... Arg> struct
	// _interface;

	template <typename _meta_info, typename return_type,
	    typename... Arg> // = meta_info
	struct _interface<_meta_info, return_type(Arg...) const> : public _meta_info {
		typedef _meta_info meta_info;
		typedef typename _meta_info::inner_pointer_type inner_pointer_type;

	    protected:
		std::string _method_name;
		std::string _return_type_name;

		boost::shared_ptr<void> _closure;
		return_type (*_transmit)(boost::shared_ptr<void> transmitter_, inner_pointer_type object_, Arg&&...);

		template <typename _object_pointer_type, typename _object_type,
		    typename _return_type, typename... _Arg>
		struct transmitter {
			typedef _object_pointer_type object_pointer_type;
			// typedef _return_type return_type;
			typedef _object_type object_type;

			return_type (object_type::*_func)(_Arg...) const;

			transmitter(return_type (object_type::*func)(_Arg...) const)
			    : _func(func)
			{
			}

			static return_type transmit(boost::shared_ptr<void> _functor, object_pointer_type _object, _Arg&&... arg)
			{
				return // static_cast<typename static_if<std::is_void<return_type>::value,
				    // void*, return_type*>::type>(_return)    // return_
				    (*pointer_out<object_type>(_object).*pointer_out<transmitter>(_functor)->_func)(std::forward<Arg>(arg)...);
			}

		    private:
			transmitter(const transmitter& fun);
		};

	    public:
		std::string method_name()
		{
			return _method_name;
		}
		std::string return_type_name()
		{
			return _return_type_name;
		}
		template <typename object_type>
		_interface(
		    return_type (object_type::*f)(Arg...) const,
		    typename pointer_type_out<object_type, inner_pointer_type>::type o,
		    std::string _method_name = "")
		    : meta_info(o, "")
		    , _method_name(_method_name)
		    , _return_type_name(typeid(typename std::remove_reference<return_type>::type).name())
		    , _closure(pointer_out<void>(boost::make_shared<transmitter<inner_pointer_type, object_type, return_type, Arg...>>(f)))
		    , _transmit(transmitter<inner_pointer_type, object_type, return_type, Arg...>::transmit)
		{
			if (!o)
				throw std::runtime_error("nullptr error");
		}
		_interface& operator=(const _interface&) = delete;

		return_type operator()(Arg&&... arg) const
		{
			return _transmit(_closure, meta_info::_object, std::forward<Arg>(arg)...);
		}
	};

	template <typename _meta_info, typename... Arg> // = meta_info
	struct _interface<_meta_info, void(Arg...)> : public _meta_info {
		typedef _meta_info meta_info;
		typedef typename _meta_info::inner_pointer_type inner_pointer_type;

	    protected:
		std::string _method_name;
		std::string _return_type_name;

		boost::shared_ptr<void> _closure;
		void (*_transmit)(boost::shared_ptr<void> transmitter_, inner_pointer_type object_, Arg&&...);
		// const size_t _parameter_num;
		// const char* arguments_types;
		// template <typename _object_type>       // typename _object_pointer_type,
		// typename _object_type, typename _return_type, typename ... _Arg
		// struct transmitter;

		template <typename _object_type,
		    typename _object_pointer_type = inner_pointer_type>
		struct transmitter {
			typedef _object_pointer_type object_pointer_type;
			typedef _object_type object_type;

			void (object_type::*_func)(Arg...);

			transmitter(void (object_type::*func)(Arg...))
			    : _func(func)
			{
			}

			static void transmit(boost::shared_ptr<void> _functor, object_pointer_type _object, Arg&&... arg)
			{
				(*pointer_out<object_type>(_object).*
				    pointer_out<transmitter>(_functor)->_func)(
				    std::forward<Arg>(arg)...);
			}

		    private:
			transmitter(const transmitter&);
		};

		// template <typename _return_type, typename ... _Arg>
		// struct transmitter_static;

		template <typename _object_pointer_type>                   // typename ... _Arg
		struct transmitter<std::nullptr_t, _object_pointer_type> { // <void, Arg ...>
			typedef _object_pointer_type object_pointer_type;
			void (*_func)(Arg...);

			transmitter(void (*func)(Arg...))
			    : _func(func)
			{
			}

			static void transmit(boost::shared_ptr<void> _functor, object_pointer_type _object, Arg&&... arg)
			{
				(void)_object; // return    // static_cast<typename
				// static_if<std::is_void<return_type>::value, void*,
				// return_type*>::type>(_return)    // return_
				(pointer_out<transmitter>(_functor)->_func)(
				    std::forward<Arg>(arg)...);
			}

		    private:
			transmitter(const transmitter&);
		};

	    public:
		std::string method_name()
		{
			return _method_name;
		}
		std::string return_type_name()
		{
			return _return_type_name;
		}
		template <typename object_type>
		_interface(
		    void (object_type::*f)(Arg...),
		    typename pointer_type_out<object_type, inner_pointer_type>::type o,
		    std::string _method_name = "")
		    : meta_info(o, "")
		    , _method_name(_method_name)
		    , _return_type_name(typeid(typename std::remove_reference<void>::type).name())
		    , _closure(pointer_out<void>(boost::make_shared<transmitter<object_type, inner_pointer_type>>(f)))
		    , _transmit(transmitter<object_type, inner_pointer_type>::transmit)
		{
			if (!o)
				throw std::runtime_error("nullptr error");
		}
		_interface(void (*f)(Arg...), std::string _method_name = "")
		    : meta_info(inner_pointer_type(nullptr), "")
		    , _method_name(_method_name)
		    , _return_type_name(typeid(void).name())
		    , _closure(pointer_out<void>(boost::make_shared<transmitter<std::nullptr_t, inner_pointer_type>>(f)))
		    , _transmit(transmitter<std::nullptr_t, inner_pointer_type>::transmit)
		{
		}

		_interface& operator=(const _interface&) = delete; // because of deep copy

		auto operator()(Arg&&... arg) -> void
		{
			return _transmit(_closure, meta_info::_object, std::forward<Arg>(arg)...);
		} // const // for serialize()
	};

	template <typename _meta_info, typename... Arg> // = meta_info
	struct _interface<_meta_info, void(Arg...) const> : public _meta_info {
		typedef _meta_info meta_info;
		typedef typename _meta_info::inner_pointer_type inner_pointer_type;

	    protected:
		std::string _method_name;
		std::string _return_type_name;

		boost::shared_ptr<void> _closure;
		void (*_transmit)(boost::shared_ptr<void> transmitter_, inner_pointer_type object_, Arg&&...);
		// const size_t _parameter_num;
		// const char* arguments_types;

		template <typename _object_pointer_type, typename _object_type, typename _return_type, typename... _Arg>
		struct transmitter;

		template <typename _object_pointer_type, typename _object_type, typename... _Arg>
		struct transmitter<_object_pointer_type, _object_type, void, _Arg...> {
			typedef _object_pointer_type object_pointer_type;
			typedef _object_type object_type;

			void (object_type::*_func)(_Arg...) const;

			transmitter(void (object_type::*func)(_Arg...) const)
			    : _func(func)
			{
			}

			static void transmit(boost::shared_ptr<void> _functor, object_pointer_type _object, _Arg&&... arg)
			{
				(*pointer_out<object_type>(_object).*
				    pointer_out<transmitter>(_functor)->_func)(
				    std::forward<Arg>(arg)...);
			}

		    private:
			transmitter(const transmitter& fun);
		};

	    public:
		std::string method_name()
		{
			return _method_name;
		}
		std::string return_type_name()
		{
			return _return_type_name;
		}
		template <typename object_type>
		_interface(
		    void (object_type::*f)(Arg...) const,
		    typename pointer_type_out<object_type, inner_pointer_type>::type o,
		    std::string _method_name = "")
		    : meta_info(o, "")
		    , _method_name(_method_name)
		    , _return_type_name(typeid(typename std::remove_reference<void>::type).name())
		    , _closure(pointer_out<void>(boost::make_shared<transmitter<inner_pointer_type, object_type, void, Arg...>>(f)))
		    , _transmit(transmitter<inner_pointer_type, object_type, void, Arg...>::transmit)
		{
			if (!o)
				throw std::runtime_error("nullptr error");
		}
		_interface& operator=(const _interface&) = delete;

		void operator()(Arg&&... arg) const
		{
			_transmit(_closure, meta_info::_object, std::forward<Arg>(arg)...);
		}
	};

	// template <typename return_type, typename ... Arg> struct _interface_static;

	// template <typename return_type, typename ... Arg> struct _interface_static
	// <return_type(Arg ...)> {
	// protected:
	// std::string _method_name;
	// std::string _return_type_name;

	// boost::shared_ptr<void> _transmitter;
	// return_type (*_transmit)(boost::shared_ptr<void> static_transmitter_, Arg
	// ...);

	// template <typename _return_type, typename ... _Arg>
	// struct transmitter {
	////                typedef _return_type return_type;

	// return_type (*_func)(_Arg ...);

	// transmitter(return_type(*func)(_Arg ...)) : _func(func)
	// {
	// }

	// static return_type transmit(boost::shared_ptr<void> _functor, _Arg ... arg)
	// {
	// return    // static_cast<typename static_if<std::is_void<return_type>::value,
	// void*, return_type*>::type>(_return)    // return_
	// (static_if_shared_pointer_cast<transmitter>(_functor)->_func)(arg ...);
	// }

	// private:
	// transmitter(const transmitter &fun);
	// };

	// public:
	// std::string method_name()
	// {
	// return _method_name;
	// }
	// std::string return_type_name()
	// {
	// return _return_type_name;
	// }

	// _interface_static(return_type(*f)(Arg ...)
	// , std::string _method_name = ""
	// )
	// : _method_name(_method_name)
	// , _return_type_name(typeid(typename
	// std::remove_reference<return_type>::type).name())
	// ,
	// _transmitter(static_if_shared_pointer_cast<void>(boost::make_shared<transmitter<return_type,
	// Arg ...> >(f)))
	// , _transmit(transmitter<return_type, Arg ...>::transmit)
	// {
	// }

	// _interface_static &operator=(const _interface_static &) = delete;

	// auto operator()(Arg ... arg)->return_type {
	// return _transmit(_transmitter, std::forward<Arg>(arg) ...);
	// }
	// };

	// template <typename ... Arg> struct _interface_static<void(Arg ...)> {
	// protected:
	// std::string _method_name;
	// std::string _return_type_name;

	// boost::shared_ptr<void> _transmitter;
	// void (*_transmit)(boost::shared_ptr<void> static_transmitter_, Arg ...);

	// template <typename _return_type, typename ... _Arg>
	// struct transmitter;

	// template <typename ... _Arg>
	// struct transmitter<void, _Arg ...> {

	// void (*_func)(_Arg ...);

	// transmitter(void(*func)(_Arg ...)) : _func(func)
	// {
	// }

	// static void transmit(boost::shared_ptr<void> _functor, _Arg ... arg)
	// {
	////return    // static_cast<typename
	///static_if<std::is_void<return_type>::value, void*,
	///return_type*>::type>(_return)    // return_
	// (static_if_shared_pointer_cast<transmitter>(_functor)->_func)(arg ...);
	// }

	// private:
	// transmitter(const transmitter &fun);
	// };

	// public:
	// std::string method_name()
	// {
	// return _method_name;
	// }
	// std::string return_type_name()
	// {
	// return _return_type_name;
	// }

	// _interface_static(void(*f)(Arg ...)
	// , std::string _method_name = ""
	// )
	// : _method_name(_method_name)
	// , _return_type_name(typeid(void).name())
	// ,
	// _transmitter(static_if_shared_pointer_cast<void>(boost::make_shared<transmitter<void,
	// Arg ...> >(f)))
	// , _transmit(transmitter<void, Arg ...>::transmit)
	// {
	// }

	// _interface_static &operator=(const _interface_static &) = delete;

	// auto operator()(Arg ... arg)->void {
	// _transmit(_transmitter, std::forward<Arg>(arg) ...);
	// }
	// };

	template <typename return_type>
	struct get_return {
		return_type rt;
		std::unique_ptr<sd::method<>> method_object;
		template <typename object_type, typename... Arg>
		get_return(std::string _method_name,
		    return_type (object_type::*f)(Arg...), // , return_type *r// = (return_type *)0
		    object_type* const o, Arg&&... arg)
		    : method_object(std::make_unique<sd::method<>>(_method_name, f, &rt, o, arg...))
		{
		}

		return_type operator()()
		{
			method_object();
			return rt;
		}
	};

#include <functional>

	template <typename _meta_info>
	struct record // template <> struct record<meta_info<shared_ptr<void>>> : public	// meta_info<shared_ptr<void>>
	{
		typedef _meta_info meta_info;
		typedef typename _meta_info::inner_pointer_type inner_pointer_type;
		typedef method<_meta_info> method_type;

		std::map<std::string, method_type*> method_field;
		// std::map<std::string, meta_info*> data_field; // just for communication

		template <typename object_type>
		record(typename pointer_type_out<object_type, inner_pointer_type>::type o, const std::string object_name = "")
		    : meta_info(o, object_name)
		// , method_field(o->memethod_field())   // this->push_back(...) do it
		// , data_field(o->data_field())
		{
		}

		method_type& operator[](const std::string method_name)
		{
			return *method_field[method_name];
		} // read/write! from/to common_data

		template <typename object_type, typename return_type = void, typename... Arg>
		method_type& push_back(const std::string _method_name, return_type (object_type::*funct)(Arg...),
		    return_type* rt, // = (return_type *)0
		    Arg&&... arg)
		{
			// assert(static_cast<object_type*>(meta_info::_object) != nullptr);
			assert(typeid(object_type).name() == meta_info::object_type());
			return *(method_field[_method_name] = new method_type(_method_name, funct, rt, static_cast<object_type*>(meta_info::_object), std::forward<Arg>(arg)...)); // fatal dangerous?
		}
		~record()
		{
			for (auto i : method_field)
				if (i.second)
					delete i.second;
		}
		std::ostream& operator<<(std::ostream& out);

		template <typename meta_info_>
		friend std::ostream& operator<<(std::ostream& out, const record<meta_info_>& dt);
		// friend void serialize(const record& item, std::ostream& out);
		// friend void deserialize(record & item, std::istream & in);
	};

	//// I am a generic object, equal to object delegate, not a switch
	//// I want there is no prototype (like C++ class for object),just this "open
	///object" itself
	//// I am a generic method inside an object
	//// inelc should make me a delegate, not a base class for inherit
	// template <> struct record<_serialize> : public _serialize
	// {
	//// If this is for current object, no "new" keyword needed
	// typedef _serialize meta_info;
	// typedef method<sd::meta_info<shared_ptr<void>>> method_type;

	// std::map < std::string, method_type* >
	// method_field;                             // it is dynamic, so it does not
	// must have stable number of members
	// std::map<std::string, meta_info*> data_field; // just for communication

	// template <typename _object_type>
	// record(
	// _object_type* o
	// , void(_object_type::*s)(std::ostream&)
	// , void(_object_type::*d)(std::istream&)
	// , std::string object_name = ""
	// )
	// : meta_info(o, s, d, object_name)   //, method_field(o->memethod_field())
	// // this->push_back(...) do it
	////, data_field(o->data_field())
	// {
	// }

	// method_type& operator[](const std::string method_name) // read/write! from/to
	// common_data
	// {
	// return *method_field[method_name]; //(*global_map[name])(this);
	// }

	// template <typename object_type, typename return_type = void, typename... Arg>
	// method_type& push_back(
	// const std::string _method_name
	// , return_type(object_type::*funct)(Arg...)
	// , return_type* rt // = (return_type *)0
	// , Arg&&... arg
	// )
	// {
	// assert(typeid(object_type).name() == meta_info::object_type());
	// return *(
	// method_field[_method_name] = new method_type(
	// _method_name
	// , funct
	// , rt
	// , static_cast<object_type*>(meta_info::_object) // fatal dangerous?
	// , std::forward<Arg>(arg)...
	// )
	// );
	// }

	////void serialize(std::ostream& out) const // message for reconstruction
	////{

	////}

	////void deserialize(std::istream& in)        // message for reconstruction
	////{

	////}

	// ~record()
	// {
	// for (auto i : method_field) {
	// if (i.second)
	// delete i.second;
	// }
	// }
	// std::ostream& operator<<(std::ostream& out);
	// friend std::ostream& operator<<(std::ostream& out, const record<method_type>&
	// dt);
	////friend void serialize(const record& item, std::ostream& out);
	////friend void deserialize(record& item, std::istream& in);
	// std::vector<std::string> tasks;
	// };

	// template <typename _meta_info> void serialize(const record<_meta_info>& item,
	// std::ostream& out)
	// {
	// item.serialize(out);
	// }

	// template <typename _meta_info> void deserialize(record<_meta_info>& item,
	// std::istream& in)
	// {
	// item.deserialize(in);
	// }

	template <typename>
	struct table;

	// template <typename _meta_info, template <typename> class _element>
	template <typename _element>
	struct table { // : public _element::meta_info
	    protected:
		friend ::boost::serialization::access;
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			(void)ar;
			(void)version;
		} // ** note that this is empty **

	    public:
		// for already defined object prototype, not all objects container with same
		// name
		// typedef typename _element::meta_info meta_info;
		typedef _element element_type;
		std::map<std::string, std::shared_ptr<element_type>> _object_map;

		table(std::string name_)
		{
			(void)name_;
		} // : meta_info(boost::shared_ptr<table>(this), name_)   // dangerous

		table(std::map<std::string, std::shared_ptr<element_type>>&& object_map_)
		    : _object_map(object_map_)
		{
		}

		element_type& operator[](std::string name)
		{
			return *_object_map[name];
		} // may be nothing!!!    // read/write! from/to

		element_type& push_back(std::shared_ptr<element_type> _record)
		{
			return *(_object_map[_record->object_name()] = _record);
		}

		~table()
		{
			_object_map.clear();
		}
	};

	////
	///http://stackoverflow.com/questions/12261915/howto-throw-stdexceptions-with-variable-messages
	//// throw std::runtime_error(sd::to_string("Error..." , fileName, " (pos: ",
	///position, ").", '!'));
	//// *The '!' char (not string) in the end is for demonstration purposes.*
	//// To archive this, I use variadic templates in the following manner:

	// template<typename T, typename ... Args>inline std::string to_string(const T
	// &prefix, const Args & ... args){return std::to_string(prefix) +
	// to_string(args ...);}
	// template<typename ... Args>inline std::string to_string(const char *prefix,
	// const Args & ... args){return std::string(prefix) + to_string(args ...);}
	// template<typename ... Args>inline std::string to_string(const std::string
	// &prefix, const Args & ... args){return prefix + to_string(args ...);}
	// template<typename ... Args>inline std::string to_string(char ch, const Args &
	// ... args){return (std::string() += ch) + to_string(args ...);}
	// template <typename T>inline std::string to_string(const T &value){return
	// std::to_string(value);}
	// inline std::string to_string(const std::string &value){return value;}
}
namespace detail {
	// http://stackoverflow.com/questions/34589458/cleanest-way-for-conditional-code-instantiation-in-c-template
	template <bool>
	struct tag {
	};

	template <typename T, typename F>
	auto static_if(tag<true>, T t, F)
	{
		return t;
	}

	template <typename T, typename F>
	auto static_if(tag<false>, T, F f)
	{
		return f;
	}

	template <bool B, typename T, typename F>
	auto static_if(T t, F f)
	{
		return static_if(tag<B>{}, t, f);
	}

	template <bool B, typename T>
	auto static_if(T t)
	{
		return static_if(tag<B>{}, t, [](auto&&...) {});
	}
}

// how about boost::shared_ptr<template<...>class>:
// http://stackoverflow.com/questions/17706319/boost-serialization-with-shared-ptr-without-implementing-serialize-function-in?rq=1

namespace boost {
	namespace serialization {
		// template<class Archive>
		// inline void save_construct_data(Archive & ar, const sd::meta_info * ei, const
		// unsigned int file_version)
		// {
		////ei->save_transmit(ar, ei->object(), file_version);
		// }

		// template<class Archive>
		// inline void load_construct_data(Archive & ar, sd::meta_info * ei, const
		// unsigned int file_version)
		// {
		////ei->load_transmit(ar, ei->object(), file_version);
		// }
	}
}

typedef sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void, std::string const&> method_interface_type;
// BOOST_SERIALIZATION_SHARED_PTR(sd::_interface<sd::meta_info, void,
// std::string const &>);
// BOOST_CLASS_TRACKING(method_interface_type,
// boost::serialization::track_always)

namespace boost {
	namespace serialization {
		// template<class Archive, typename _meta_info>
		// inline void save_construct_data(Archive & ar, const sd::method<_meta_info> *
		// ei, const unsigned int file_version)
		// {
		// save_construct_data(ar, static_cast<const _meta_info*>(ei), file_version);
		// // static_cast<_meta_info*>(ei)->_save_transmit(ar,
		// static_cast<_meta_info*>(ei)->object(), file_version);
		// }

		// template<class Archive, typename _meta_info>
		// inline void load_construct_data(Archive & ar, sd::method<_meta_info> * ei,
		// const unsigned int file_version)
		// {
		// load_construct_data(ar, static_cast<_meta_info*>(ei), file_version);    //
		// static_cast<_meta_info*>(ei)->_load_transmit(ar,
		// static_cast<_meta_info*>(ei)->object(), file_version);
		// }

		// template<class Archive, typename _meta_info>
		// inline void save(Archive & ar, const ::boost::shared_ptr <
		// sd::method<_meta_info>> & t, const unsigned int file_version)
		// {
		// const sd::method<_meta_info> * t_ptr = t.get();
		//// just serialize the underlying raw pointer
		// ar << boost::serialization::make_nvp("px", t_ptr);
		// }

		// template<class Archive, typename _meta_info>
		// inline void load(Archive & ar, ::boost::shared_ptr < sd::method<_meta_info>>
		// & t, const unsigned int file_version)
		// {
		// sd::_interface<_meta_info> * r;
		//// recover the underlying raw pointer
		// ar >> boost::serialization::make_nvp("px", r);

		//// To Do - match up with other shared pointers which
		//// use this same raw pointer.

		// }
	}
}

// BOOST_SERIALIZATION_SHARED_PTR(template <
// typename _meta_info
// , typename return_type
// , typename... Arg
// >
// struct _interface);

// BOOST_CLASS_TRACKING(template <
// typename _meta_info
// , typename return_type
// , typename... Arg
// >
// struct _interface, boost::serialization::track_always);

namespace boost {
	namespace serialization {
	}
}

#endif // DELEGATE_H_INCLUDED
