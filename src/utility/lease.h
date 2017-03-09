#ifndef LEASE_H
#define LEASE_H

#include "utility/delegate.h"
#include <QObject>
#include <boost/assert.hpp>
#include <cassert>
#include <memory>

struct can_rent : public QObject {

	Q_OBJECT
    public:
	can_rent(QObject* parent = nullptr)
	    : QObject(parent) {}
	~can_rent() { emit close_request(this); }
    signals:
	void close_request(QObject*);
};

// deprecated
struct borrower_slot : public QObject {
	Q_OBJECT
    public:
	borrower_slot(QObject* rt)
	    : borrowed(rt) {}
    protected:
	//	std::map<QString, QObject*> borrowed_map;
	QObject* borrowed;
    public slots:
	void on_close_request(QObject* renter_)
	{
		//		assert(o == borrowed_map[o->objectName()]);
		//		borrowed_map[o->objectName()] = nullptr;
		assert(renter_ == borrowed);
		borrowed = nullptr;
	}
};

template <typename borrowed_type>
struct borrower // : public borrower
{
    public:
	borrower(can_rent* rt)
	    : borrowed(rt) //borrower_slot(rt)
	{
	}


	void on_close_request(QObject* renter_)
	{
		//		assert(o == borrowed_map[o->objectName()]);
		//		borrowed_map[o->objectName()] = nullptr;
		assert(renter_ == borrowed);
		borrowed = nullptr;
	}

	//	template <typename return_type, typename... arguments>
	//	return_type operator()(return_type (borrowed_type::*function_)(arguments...), arguments... args)
	//	{
	//		if (nullptr != borrowed)
	//			return (static_cast<borrowed_type*>(borrowed)->*function_)(args...);
	//		else
	//		    return return_type();
	//	}

	template <typename return_type, typename... arguments>
	void operator()(return_type& return_, return_type (borrowed_type::*function_)(arguments...), arguments... args)
	{
		if (nullptr != borrowed)
			return_ = (static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<arguments>(args)...);
	}

	template <typename return_type, typename... arguments>
	void operator()(return_type& return_, return_type (borrowed_type::*function_)(const arguments&...), const arguments&... args)
	{
		if (nullptr != borrowed)
			return_ = (static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<const arguments&>(args)...);
	}


	template <typename return_type, typename... arguments>
	void operator()(return_type& return_, return_type (borrowed_type::*function_)(arguments...) const, arguments... args)
	{
		if (nullptr != borrowed)
			return_ = (static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<arguments>(args)...);
	}

	template <typename return_type, typename... arguments>
	void operator()(return_type& return_, return_type (borrowed_type::*function_)(const arguments&...) const, const arguments&... args)
	{
		if (nullptr != borrowed)
			return_ = (static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<const arguments&>(args)...);
	}

	template <typename return_type, typename... arguments>
	void operator()(return_type (borrowed_type::*function_)(arguments...), arguments... args)
	{
		if (nullptr != borrowed)
			(static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<arguments>(args)...);
	}

	template <typename return_type, typename... arguments>
	void operator()(return_type (borrowed_type::*function_)(const arguments&...), const arguments&... args)
	{
		if (nullptr != borrowed)
			(static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<const arguments&>(args)...);
	}

	template <typename return_type, typename... arguments>
	void operator()(return_type (borrowed_type::*function_)(arguments...) const, arguments... args)
	{
		if (nullptr != borrowed)
			(static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<arguments>(args)...);
	}

	template <typename return_type, typename... arguments>
	void operator()(return_type (borrowed_type::*function_)(const arguments&...) const, const arguments&... args)
	{
		if (nullptr != borrowed)
			(static_cast<borrowed_type*>(borrowed)->*function_)(std::forward<const arguments&>(args)...);
	}

    protected:
	//	std::map<QString, QObject*> borrowed_map;
	QObject* borrowed;
};

template <typename borrowed_type>
std::shared_ptr<borrower<borrowed_type>> real_renter_constructin(borrowed_type* renter_)
{
	BOOST_MPL_ASSERT_RELATION((sd::equal_type<typename borrowed_type::super, can_rent>::value), ==, true);
	static_assert(sd::equal_type<typename borrowed_type::super, can_rent>::value == true, "borrowed_type is not a object which can be rent out");
	auto borrower_ = std::make_shared<borrower<borrowed_type>>(renter_);
	QObject::connect(renter_, &can_rent::close_request, [=](QObject* renter_) { borrower_->on_close_request(renter_); });
	return borrower_;
}


#endif // LEASE_H
