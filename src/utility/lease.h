#ifndef LEASE_H
#define LEASE_H

#ifdef USE_SD_METHOD
#include "utility/delegate.h"
#endif //USE_SD_METHOD
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <cassert>
#include <functional>
#include <memory>
#include <mutex>

template <typename real_renter>
struct can_rent // : public QObject
{
	//	Q_OBJECT
    public:
	can_rent(real_renter* renter_)
	    : //QObject(nullptr),
	    _renter(renter_)
	{
	}
	~can_rent() { _renter->close_request(_renter); }
	//    signals:
	void close_request(real_renter*);

    private:
	real_renter* _renter;
};

//// deprecated
//struct borrower_slot : public QObject {
//	Q_OBJECT
//    public:
//	borrower_slot(QObject* rt)
//	    : borrowed(rt) {}
//    protected:
//	//	std::map<QString, QObject*> borrowed_map;
//	QObject* borrowed;
//    public slots:
//	void on_close_request(QObject* renter_)
//	{
//		//		assert(o == borrowed_map[o->objectName()]);
//		//		borrowed_map[o->objectName()] = nullptr;
//		assert(renter_ == borrowed);
//		borrowed = nullptr;
//	}
//};

namespace sd {

	//	template <typename real_renter>
	class renter;
}
//class Blogger;
//class rs_t;
//class rctrl_t;
//namespace web {
//	class Browser;
//	class TabWidget;
//	class ChaseWidget;
//}
namespace boost {
	namespace sp_adl_block {
		//		template <typename CounterPolicyT>                                                                                  //typename DerivedT,
		//		inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<sd::renter, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<sd::renter<Blogger>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	CounterPolicyT::increment(p->m_ref_counter);
		//		}

		//		template <typename CounterPolicyT>                                                                                  //typename DerivedT,
		//		inline void intrusive_ptr_release(const boost::intrusive_ref_counter<sd::renter, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_release(const boost::intrusive_ref_counter<sd::renter<Blogger>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	if (CounterPolicyT::decrement(p->m_ref_counter) == 0)
		//			//	    delete static_cast< const DerivedT* >(p);
		//		}

		//		template <typename CounterPolicyT>                                                                               //typename DerivedT,
		//		inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<Blogger, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<sd::renter<Blogger>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	CounterPolicyT::increment(p->m_ref_counter);
		//		}

		//		template <typename CounterPolicyT>                                                                               //typename DerivedT,
		//		inline void intrusive_ptr_release(const boost::intrusive_ref_counter<Blogger, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_release(const boost::intrusive_ref_counter<sd::renter<Blogger>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	if (CounterPolicyT::decrement(p->m_ref_counter) == 0)
		//			//	    delete static_cast< const DerivedT* >(p);
		//		}
		//		template <typename CounterPolicyT>                                                                            //typename DerivedT,
		//		inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<rs_t, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<sd::renter<rs_t>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	CounterPolicyT::increment(p->m_ref_counter);
		//		}

		//		template <typename CounterPolicyT>                                                                            //typename DerivedT,
		//		inline void intrusive_ptr_release(const boost::intrusive_ref_counter<rs_t, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_release(const boost::intrusive_ref_counter<sd::renter<rs_t>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	if (CounterPolicyT::decrement(p->m_ref_counter) == 0)
		//			//	    delete static_cast< const DerivedT* >(p);
		//		}
		//		template <typename CounterPolicyT>                                                                               //typename DerivedT,
		//		inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<rctrl_t, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<sd::renter<rctrl_t>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	CounterPolicyT::increment(p->m_ref_counter);
		//		}

		//		template <typename CounterPolicyT>                                                                               //typename DerivedT,
		//		inline void intrusive_ptr_release(const boost::intrusive_ref_counter<rctrl_t, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_release(const boost::intrusive_ref_counter<sd::renter<rctrl_t>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	if (CounterPolicyT::decrement(p->m_ref_counter) == 0)
		//			//	    delete static_cast< const DerivedT* >(p);
		//		}

		//		template <typename CounterPolicyT>                                                                                    //typename DerivedT,
		//		inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<web::Browser, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<sd::renter<web::Browser>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	CounterPolicyT::increment(p->m_ref_counter);
		//		}

		//		template <typename CounterPolicyT>                                                                                    //typename DerivedT,
		//		inline void intrusive_ptr_release(const boost::intrusive_ref_counter<web::Browser, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_release(const boost::intrusive_ref_counter<sd::renter<web::Browser>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	if (CounterPolicyT::decrement(p->m_ref_counter) == 0)
		//			//	    delete static_cast< const DerivedT* >(p);
		//		}

		//		template <typename CounterPolicyT>                                                                                      //typename DerivedT,
		//		inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<web::TabWidget, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_add_ref(const boost::intrusive_ref_counter<sd::renter<web::TabWidget>, CounterPolicyT>* p) BOOST_NOEXCEPT//DerivedT
		//		{
		//			(void)p;
		//			//	CounterPolicyT::increment(p->m_ref_counter);
		//		}

		//		template <typename CounterPolicyT>                                                                                      //typename DerivedT,
		//		inline void intrusive_ptr_release(const boost::intrusive_ref_counter<web::TabWidget, CounterPolicyT>* p) BOOST_NOEXCEPT //inline void intrusive_ptr_release(const boost::intrusive_ref_counter<sd::renter<web::TabWidget>, CounterPolicyT>* p) BOOST_NOEXCEPT //DerivedT
		//		{
		//			(void)p;
		//			//	if (CounterPolicyT::decrement(p->m_ref_counter) == 0)
		//			//	    delete static_cast< const DerivedT* >(p);
		//		}
	} //sp_adl_block
} //boost


namespace sd {


	//    template<typename _Tp, typename... _Args>
	//      inline shared_ptr<_Tp>
	//      make_shared(_Args&&... __args)
	//      {
	//	typedef typename std::remove_const<_Tp>::type _Tp_nc;
	//	return std::allocate_shared<_Tp>(std::allocator<_Tp_nc>(),
	//					 std::forward<_Args>(__args)...);
	//      }

	// Could we use some compiler extesion to provide this base to traditional code base?
	//	template <typename real_renter>
	class renter;

	template <typename real_renter> //, template <typename> class renter_t = renter
	struct intrusive_ptr;

	//	template <typename real_renter>
	class renter : public boost::intrusive_ref_counter<renter, boost::thread_safe_counter> //boost::intrusive_ref_counter<real_renter, boost::thread_safe_counter>
	{
	    public:
		typedef typename boost::signals2::signal<void(renter* const)> signal_type;
		typedef typename boost::signals2::signal<void(renter* const)>::slot_type slot_type;
		//		typedef real_renter element_type;
		//		typedef boost::thread_safe_counter counter_policy;

		//		void close_request(renter** const self_reference)
		//		{
		//			*self_reference = nullptr;
		//			_close_request(static_cast<element_type* const>(this));
		//		}
		//		boost::signals2::signal<void(element_type* const)> close_requested;
		//		template <typename renter_type>


		//		void close_trigger_from_others(renter* const sender_)
		//		{
		//			if (!_close_request_triggered && !_destroyed && sender_ != this) {
		//				_close_request(sender_); // close_request_sent = true;
		//			}
		//		}

		auto close_trigger_from_others() //sd::intrusive_ptr<renter_type>& this //(renter_type *const receiver_)
		{
			return [&](renter* const sender_) { // if (this) { //boost::intrusive_ptr<real_sender>(receiver_).get()
				if (!this->_close_request_sent && !this->_destroy_request_sent && sender_ != this) {
					_close_request_sent = true;
					this->_close_request(sender_); //
								       //					_close_request.disconnect_all_slots();
				}                                      // }
			};
		}

		auto destroy_trigger_from_others()
		{
			return [&](renter* const sender_) {
				if (!_destroy_request_sent && sender_ != this) {
					_destroy_request_sent = true;
					_destroy_request(sender_); //
								   //					_destroy_request.disconnect_all_slots();
				}
			};
		}


		void destroy_transfer(const std::function<void(renter* const)>& fun)
		{
			_destroy_request.connect(fun, boost::signals2::at_front);
		}

		//		template <typename renter_type>
		//		void destroy_transfer(renter_type* renter_)
		//		{
		//			_destroy_request.connect(renter_->destroy_trigger_from_others());
		//		}

		~renter()
		{
			//			boost::intrusive_ptr_release<renter, counter_policy>(this);
			// for debug
			// self_close_request_impl(this);
			if (!_close_request_sent || !_destroy_request_sent) {
				_destroy_request_sent = true;
				_destroy_request(this); //if (!_close_request_sent && !_closed) _close_request(this);
							//				_destroy_request.disconnect_all_slots();
			}
		}

		bool close_request_sent() const { return _close_request_sent; }

		bool destroy_request_sent() const { return _destroy_request_sent; }
	    protected:
		//		void close_request_received(bool status) { _close_request_triggered = status; }
		//		void is_destroyed(bool status) { _destroyed = status; }
		renter()
		{
			//			//			boost::intrusive_ptr_add_ref<renter, counter_policy>(this);
			//			//			static_assert(std::is_base_of<renter, element_type>::value, "element_type needs to be a renter");
			_close_request.connect(std::bind([&] { _close_request_sent = true; }));
			_destroy_request.connect(std::bind([&] {
				_destroy_request_sent = true;
				if (!_close_request_sent) {
					_close_request_sent = true;
					_close_request(this);
					//					_close_request.disconnect_all_slots();
				}
			}));
			//			close_requested.connect(std::bind(&renter::self_close_request_impl, this, std::placeholders::_1));
		}


		//	    private:
		//		void self_close_request_impl(renter* const sender_)
		//		{
		//			if (!close_request_sent && sender_ != this) {
		//				_close_request(static_cast<element_type* const>(this));
		//				//                                close_request_sent = true;
		//			}
		//		}
		//		real_renter* _renter_inside;
		boost::signals2::signal<void(renter* const)> _close_request; // http://stackoverflow.com/questions/14758088/how-are-atomic-operations-implemented-at-a-hardware-level
		//		boost::signals2::scoped_connection _close_request_connection;

		//		template <typename r, template <typename> class br>
		//		friend struct intrusive_ptr;

		//		friend struct intrusive_ptr<element_type>; //, sd::renter
		template <typename element_type>
		friend struct intrusive_ptr;

		bool _close_request_sent = false;
		//		std::map<size_t, std::shared_ptr<sd::method<sd::meta_info<void>>>> _slots;
		bool _destroy_request_sent = false;

	    private:
#ifdef USE_SD_METHOD
		//		template <typename bound>
		void close_reveive(std::shared_ptr<sd::method<sd::meta_info<void>>> mt)
		{
			//			//			auto index = _slots.size();
			//			//			_slots[index] = std::forward<std::shared_ptr<sd::method<sd::meta_info<void>>>>(mt);
			//			_close_request.connect(std::bind([&, mt] { // index,
			//				(*mt)();                           //_slots[index]->operator()();
			//			}));
			_close_request.connect([&, mt](renter* const) { (*mt)(); });
			//			_close_request.connect([=](real_renter*) { std::forward<const sd::method<bound>>(mt)(); });
		}
#endif // USE_SD_METHOD
		void close_transfer(const std::function<void(renter* const)>& fun)
		{
			_close_request.connect(fun, boost::signals2::at_front);
		}

		boost::signals2::signal<void(renter* const)> _destroy_request;
		//		boost::signals2::scoped_connection _destroy_request_connection;
	};

	struct receiver_status {
	    protected:
		bool _close_request_received = false;
		bool _destroyed_request_received = false;
	};

	template <class T, class U>
	void renew_connection(intrusive_ptr<T>& left, intrusive_ptr<U>& right);

	template <typename real_renter>                                     //, template <typename> class renter_t
	struct intrusive_ptr :                                              // think about std::shared_ptr
			       protected boost::intrusive_ptr<real_renter>, // public std::shared_ptr<real_renter> //std::enable_shared_from_this<shared_ptr<real_renter>> // : public borrower
			       protected receiver_status {
	    public:
		typedef real_renter element_type;
		typedef typename renter::slot_type slot_type;

	    private:
		typedef intrusive_ptr this_type;

		bool internal_integrity() const
		{
			auto //px_pre
			    px = boost::intrusive_ptr<element_type>::get();
			static_assert(std::is_base_of<renter, real_renter>::value, "miss match base type!");
			//			auto px = static_cast<renter*>(px_pre);
			bool conflict = false;
			if (!px //|| px->_close_request_sent || px->_destroy_request_sent
			    ) conflict = (_shadow != nullptr);
			//			bool null = !px && !_shadow;
			bool closed = !_shadow; //px && (px->_close_request_triggered || px->_destroyed_request_triggered) && !_shadow;

			//			auto _shadow_cast = static_cast<renter*>(_shadow);
			bool normal = px && !px->_close_request_sent && !px->_destroy_request_sent && _shadow && (_shadow //_shadow_cast
														     == px);
			//			assert(( //null ||
			//				   closed || normal) &&
			//			    !conflict);
			return //null ||
			    (closed || normal) && !conflict;
		}

	    public:
		~intrusive_ptr()
		{ //: boost::intrusive_ptr<element_type>::~intrusive_ptr()
			//			delete boost::intrusive_ptr<element_type>::get();
			//			if(!internal_integrity());
			if (_shadow != nullptr) {
				//				detach();}
				//			else {
				_shadow = nullptr;
				//				_destroyed_request_received = true;
				//				assert(internal_integrity());
			}
			detach();
			_destroyed_request_received = true;
			assert(internal_integrity());
		}


		explicit intrusive_ptr(std::nullptr_t) // = delete;
		    : boost::intrusive_ptr<element_type>(nullptr),
		      _shadow(nullptr)
		{
			assert(internal_integrity());
		}
		// un-default constructable object will fail
		// semantic wrong if use detach, fail if use swap on uninitialized object.
		intrusive_ptr& operator=(std::nullptr_t) = delete;
		//		{
		//			this->operator=(static_cast<element_type*>(nullptr));
		//			//			this->detach();
		//			//			//			intrusive_ptr().swap(*this); //intrusive_ptr(nullptr).swap(*this); //
		//			//			//			intrusive_ptr(static_cast<element_type*>(0), false).swap(*this);
		//			//			//			*static_cast<::boost::intrusive_ptr<element_type>*>(this) = boost::intrusive_ptr<element_type>(static_cast<element_type*>(0), false); //->reset(static_cast<element_type*>(0), false);
		//			//			//			this->_shadow = nullptr;
		//			assert(internal_integrity());
		//			return *this;
		//		}

		explicit intrusive_ptr() //= delete;
		    : boost::intrusive_ptr<element_type>(nullptr),
		      _shadow(nullptr)
		{
			assert(internal_integrity());
		}


		void destroy_receive(const std::function<void(renter* const)>& fun //, bool force = false
		    )
		{
			if (_shadow && internal_integrity() //&& (!_destroy_request_connection.connected() || force)
			    ) {
				//				if (_destroy_request_connection.connected()) _destroy_request_connection.disconnect();// might trigger undefined error of object
				//				_destroy_request_connection = // boost::intrusive_ptr<element_type>::get()
				_shadow->_destroy_request.connect(slot_type(fun).track(this->get()->_destroy_request));
			}
		}
		//		explicit
		explicit intrusive_ptr(element_type* rhs, bool add_ref = true)
		    : boost::intrusive_ptr<element_type>(rhs, add_ref) //std::shared_ptr<element_type>(std::forward<element_type*>(rt))
		    , _shadow(rhs)
		    , _close_request_connection(_shadow->_close_request.connect(std::bind(&intrusive_ptr::on_close_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		    , _destroy_request_connection(_shadow->_destroy_request.connect(std::bind(&intrusive_ptr::on_destroy_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		{ // , _renter_inside(rt), _close_request(new boost::signals2::signal<void(element_type*)>())


			//			if (rhs) {
			//				// http://stackoverflow.com/questions/24366160/safely-disconnecting-from-boostsignals2
			//				close_receive(                                                                 //slot_type(
			//				    std::bind(&intrusive_ptr::on_close_requested, this, std::placeholders::_1) // ).track(this)// sd::intrusive_ptr<element_type>(this->get()),
			//				    );
			//				destroy_receive(                                                                 //slot_type(
			//				    std::bind(&intrusive_ptr::on_destroy_requested, this, std::placeholders::_1) // ).track(this)// sd::intrusive_ptr<element_type>(*this),
			//				    );
			//			}
			assert(internal_integrity());
		}


#if !defined(BOOST_NO_MEMBER_TEMPLATES) || defined(BOOST_MSVC6_MEMBER_TEMPLATES)

		template <class U>
#if !defined(BOOST_SP_NO_SP_CONVERTIBLE)

		explicit intrusive_ptr(intrusive_ptr<U> const& rhs, typename boost::detail::sp_enable_if_convertible<U, element_type>::type convertable_ = boost::detail::sp_empty() // ) = delete;
		    )

#else

		intrusive_ptr(intrusive_ptr<U> const& rhs //) = delete;
		    )

#endif
		    : boost::intrusive_ptr<element_type>(rhs, convertable_)
		    , _shadow(rhs._shadow)
		    , _close_request_connection(_shadow->_close_request.connect(std::bind(&intrusive_ptr::on_close_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		    , _destroy_request_connection(_shadow->_destroy_request.connect(std::bind(&intrusive_ptr::on_destroy_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		{
			//			if (rhs._shadow) {
			//				renew_connection(*this, rhs);
			//				//			_shadow = boost::intrusive_ptr<element_type>::get(); //if (px != 0) intrusive_ptr_add_ref(px);
			//			}
			assert(internal_integrity());
		}

#endif

		explicit intrusive_ptr(intrusive_ptr const& rhs // ) = delete;
		    )
		    : boost::intrusive_ptr<element_type>(rhs)
		    , _shadow(rhs._shadow)
		    , _close_request_connection(_shadow->_close_request.connect(std::bind(&intrusive_ptr::on_close_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		    , _destroy_request_connection(_shadow->_destroy_request.connect(std::bind(&intrusive_ptr::on_destroy_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		{
			//			if (rhs._shadow) {
			//				renew_connection<element_type, element_type>(*this, rhs);
			//				//			_shadow = boost::intrusive_ptr<element_type>::get(); //if (px != 0) intrusive_ptr_add_ref(px);
			//				//			assert(
			//			}
			assert(internal_integrity());
			//				    );
		}


#if !defined(BOOST_NO_MEMBER_TEMPLATES) || defined(BOOST_MSVC6_MEMBER_TEMPLATES)

		template <class U>
		intrusive_ptr& operator=(intrusive_ptr<U> const& rhs) // = delete;
		{
			U* ret = nullptr;
			if (rhs.get()) {
				ret = rhs.get();
			}

			this->operator=(ret);
			//			{
			//				detach();
			//				//			renew_connection(*this, rhs);
			//				this_type(rhs).swap(*this);
			//			}
			assert(internal_integrity());
			return *this;
		}

#endif

// Move support

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

		explicit intrusive_ptr(intrusive_ptr&& rhs) BOOST_NOEXCEPT
		    : boost::intrusive_ptr<element_type>(std::forward<boost::intrusive_ptr<element_type>>(rhs)), //[&] {renew_connection(*this, rhs);return std::forward<boost::intrusive_ptr<element_type>>(rhs); }()
		      _shadow(std::forward<element_type*>(rhs._shadow)),
		      _close_request_connection(_shadow->_close_request.connect(std::bind(&intrusive_ptr::on_close_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1))),
		      _destroy_request_connection(_shadow->_destroy_request.connect(std::bind(&intrusive_ptr::on_destroy_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		{
			//                        auto px = this->_shadow; // boost::intrusive_ptr<element_type>::get();
			//                        if (px) {
			//                                //				if (rhs._close_request_connection.connected())
			//                                _close_request_connection = px->_close_request.connect(std::bind(&intrusive_ptr<element_type>::on_close_requested, this, std::placeholders::_1));
			//                                //				if (rhs._destroy_request_connection.connected())
			//                                _destroy_request_connection = px->_destroy_request.connect(std::bind(&intrusive_ptr<element_type>::on_destroy_requested, this, std::placeholders::_1));
			//                        }
			//(std::forward<element_type*>(rhs._shadow))


			//			if (rhs._close_request_connection.connected()) rhs._close_request_connection.disconnect();
			//			if (rhs._destroy_request_connection.connected()) rhs._destroy_request_connection.disconnect();
			rhs._shadow = nullptr; //rhs.px = 0;

			assert(internal_integrity());
		}

		intrusive_ptr& operator=(intrusive_ptr&& rhs) BOOST_NOEXCEPT
		{

			assert(rhs.internal_integrity());
			element_type* ret = nullptr;
			if (rhs.boost::intrusive_ptr<element_type>::get()) {


				//				if (rhs._close_request_connection.connected()) rhs._close_request_connection.disconnect();
				//				if (rhs._destroy_request_connection.connected()) rhs._destroy_request_connection.disconnect();
				//			assert(internal_integrity());// might be uninitialized
				ret = rhs.boost::intrusive_ptr<element_type>::detach();
				//			element_type* ret = _shadow;
			}

			this->operator=(ret);
			//			{
			//				//                        auto px =
			//				//			this->reset(); //
			//				detach();
			//				this_type(static_cast<intrusive_ptr&&>(rhs)).swap(*this);
			//			}
			assert(internal_integrity());
			return *this;
		}

#endif

		intrusive_ptr& operator=(intrusive_ptr const& rhs) // = delete;
		{

			assert(rhs.internal_integrity());

			this->operator=(rhs._shadow);
			//			{
			//				detach(); //			renew_connection(*this, rhs);
			//				assert(rhs.internal_integrity());

			//				this_type(rhs).swap(*this);
			//			}


			assert(internal_integrity());
			return *this;
		}

		element_type* get() const BOOST_NOEXCEPT
		{
			assert(internal_integrity());
			return _shadow;
		}

		//		explicit
		operator element_type*()
		{
			if (!internal_integrity()) _shadow = nullptr;
			return this->get();
		}

		operator element_type*() const
		{
			if (!internal_integrity()) const_cast<intrusive_ptr*>(this)->_shadow = nullptr;
			return this->get();
		}

		void reset() BOOST_NOEXCEPT
		{
			intrusive_ptr().swap(*this);
			//			_shadow = nullptr;
			assert(_shadow == nullptr);
			assert(internal_integrity());
		}

		void reset(element_type* rhs)
		{
			intrusive_ptr(rhs).swap(*this);
			_shadow = rhs;
			assert(internal_integrity());
		}

		void reset(element_type* rhs, bool add_ref)
		{
			intrusive_ptr(rhs, add_ref).swap(*this);
			_shadow = rhs;
			assert(internal_integrity());
		}


		element_type* detach() BOOST_NOEXCEPT
		{
			element_type* ret = nullptr;
			if (boost::intrusive_ptr<element_type>::get()) {
				//				if (this->_close_request_connection.connected()) this->_close_request_connection.disconnect();
				//				if (this->_destroy_request_connection.connected()) this->_destroy_request_connection.disconnect();
				//			assert(internal_integrity());// might be uninitialized
				ret = boost::intrusive_ptr<element_type>::detach();
				//			element_type* ret = _shadow;
			}
			//			if (this->_close_request_connection.connected()) this->_close_request_connection.disconnect();
			//			if (this->_destroy_request_connection.connected()) this->_destroy_request_connection.disconnect();
			_shadow = nullptr;
			_destroyed_request_received = true;
			assert(internal_integrity());

			return ret;
		}

	    protected:
		void close_receive(const std::function<void(renter* const)>& fun //, bool force = false
		    )
		{

			if (_shadow && internal_integrity()) { // && (!_close_request_connection.connected() || force)
							       //				if (_close_request_connection.connected()) _close_request_connection.disconnect();//might trigger undefined error
							       //				_close_request_connection = //boost::intrusive_ptr<element_type>::get()
				_shadow->_close_request.connect(slot_type(fun).track(this->get()->_close_request));
			}
		}


		explicit intrusive_ptr(element_type& rhs, bool add_ref = true)
		    : boost::intrusive_ptr<element_type>(&rhs, add_ref) //std::shared_ptr<element_type>(std::forward<element_type*>(rt))
		    , _shadow(&rhs)
		    , _close_request_connection(_shadow->_close_request.connect(std::bind(&intrusive_ptr::on_close_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		    , _destroy_request_connection(_shadow->_destroy_request.connect(std::bind(&intrusive_ptr::on_destroy_requested,
			  this, //sd::intrusive_ptr<element_type>(*this),
			  std::placeholders::_1)))
		{ // , _renter_inside(rt), _close_request(new boost::signals2::signal<void(element_type*)>())
#ifdef USE_SIGNAL_TRACK

			if (&rhs) {
				// http://stackoverflow.com/questions/24366160/safely-disconnecting-from-boostsignals2
				//				_close_request_connection =
				rhs._close_request.connect( //
				    slot_type(
					//					std::bind(
					&intrusive_ptr::on_close_requested,
					this->get(), //sd::intrusive_ptr<element_type>(*this),
					std::placeholders::_1)
					//											    )
					.track(this));
				//				_destroy_request_connection =
				rhs._destroy_request.connect( //
				    slot_type(
					//					std::bind(
					&intrusive_ptr::on_destroy_requested,
					this->get(), //sd::intrusive_ptr<element_type>(*this),
					std::placeholders::_1)
					//											    )
					.track(this));
			}
#endif // USE_SIGNAL_TRACK
			assert(internal_integrity());
		}


		//#ifdef TEST
		void on_close_requested(renter* sender_)
		{                      //(element_type* original_)
			(void)sender_; // might be others on current implement
			//			//			//		assert(o == borrowed_map[o->objectName()]); borrowed_map[o->objectName()] = nullptr;
			//			//			assert(_renter == boost::intrusive_ptr<element_type>::get()); //Always fail. When signal sent, original object maybe on distruction operation
			//			//			assert(nullptr != _renter);     //Always fail. When signal sent, original object maybe on distruction operation
			//			//			assert(renter_ == _renter);     //Always fail. When signal sent, original object maybe on distruction operation
			//			// When signal sent, original object maybe on distruction operation. At current moment, _renter is undefined
			//			assert(original_ && "When signal sent, original object might be on distructing operation or not. At current moment, Current _shadow pointer, one copy of the element_type* normally is undefined. With this original pointer, you still can do something here that is not too late!");
			//			//			(void)renter_; //With this pointer, you still can do something here that is not too late!
			//			boost::intrusive_ptr<element_type>::detach();
			//			this->reset(); //
			detach(); //_shadow = nullptr;
			//			if (original_) {
			//				if (!original_->close_request_received()) {
			////					original_->close_request_received(true); // not unique, would be called multiple times
			//				}
			//			}
			//			//			assert(internal_integrity());
			//			sender_->_close_request_sent = true;
			_close_request_received = true;
			assert(internal_integrity());
		}

		void on_destroy_requested(renter* sender_)
		{
			(void)sender_;
			//			if (original_) {
			//				if (!original_->destroy_request_sent()) {
			//					//					original_->is_destroyed(true);
			//					intrusive_ptr_release(_shadow);
			//				}
			//			}

			//			this->reset(); //
			detach(); //_shadow = nullptr;
				  //			sender_->_destroyed_request_sent = true;

			assert(internal_integrity());
		}
		//#endif //TEST
		//	std::map<QString, QObject*> borrowed_map;
		element_type* _shadow = nullptr;

		boost::signals2::scoped_connection _close_request_connection;
		boost::signals2::scoped_connection _destroy_request_connection;
		//		element_type* _renter_inside;
		//		boost::signals2::signal<void(element_type*)>* _close_request;
		//	    private:
		//		element_type* px;

	    public:
		//		intrusive_ptr(std::shared_ptr<element_type>&& ptr)
		//		    :                  //std::shared_ptr<element_type>(std::forward<std::shared_ptr<element_type>>(ptr))
		//				       //,
		//		    _renter(ptr.get()) //borrower_slot(rt)
		//		    , _close_request(std::make_shared<boost::signals2::signal<void(element_type*)>>())
		//		{
		//			_close_request->connect(std::bind(&intrusive_ptr<element_type>::on_close_requested, this, std::placeholders::_1));
		//		}

		//		inline element_type* operator()() { return _renter; }
		//		inline element_type* operator&() { return _renter; }
		//		inline element_type* operator->() { return _renter; }
		//		inline element_type& operator*() { return *_renter; }
		intrusive_ptr& operator=(element_type* rhs)
		{
			if (this->_shadow) {
				this->detach(); //*?
				assert(internal_integrity());

				//				//			renew_connection(*this, this_type(rhs));
				//				this_type(rhs).swap(*this);
				//				//			_shadow = boost::intrusive_ptr<element_type>::get();
			}

			boost::intrusive_ptr<element_type>(rhs).swap(static_cast<boost::intrusive_ptr<element_type>&>(*this));
			this->_shadow = rhs;
			if (rhs) {
				this->close_receive(                                                           //slot_type(
				    std::bind(&intrusive_ptr::on_close_requested, this, std::placeholders::_1) //, //).track(&rhs)// true
				    );
				//                                rhs._close_request_connection = rhs_px->_close_request.connect(std::bind(&intrusive_ptr::on_close_requested, &rhs, std::placeholders::_1));
				this->destroy_receive(                                                           //slot_type(
				    std::bind(&intrusive_ptr::on_destroy_requested, this, std::placeholders::_1) //).track(&rhs)
				    );
				//                                rhs._destroy_request_connection = rhs_px->_destroy_request.connect(std::bind(&intrusive_ptr::on_destroy_requested, &rhs, std::placeholders::_1));
				assert(_shadow == boost::intrusive_ptr<element_type>::get());
			}


			assert(internal_integrity());
			return *this;
		}

		inline element_type* operator->() const
		{
			assert(_shadow); // if this occurred, it means you use an object already died
			assert(_shadow == boost::intrusive_ptr<element_type>::get());
			assert(internal_integrity());
			return _shadow;
		}
		inline element_type& operator*() const
		{
			assert(_shadow); // if this occurred, it means you use an object already died
			assert(_shadow == boost::intrusive_ptr<element_type>::get());
			assert(internal_integrity());
			return *_shadow;
		}

//// implicit conversion to "bool"
//#include <boost/smart_ptr/detail/operator_bool.hpp>
#if !defined(BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS) && !defined(BOOST_NO_CXX11_NULLPTR)

		explicit operator bool() const BOOST_NOEXCEPT
		{
			assert(internal_integrity());
			return _shadow != nullptr; //px != 0;
		}
#elif (defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, < 0x570)) || defined(__CINT__)
		operator bool() const BOOST_NOEXCEPT
		{
			assert(internal_integrity());
			return _shadow != nullptr;
		}

#elif defined(_MANAGED)

		static void unspecified_bool(this_type***)
		{
		}

		typedef void (*unspecified_bool_type)(this_type***);

		operator unspecified_bool_type() const BOOST_NOEXCEPT
		{
			return _shadow == 0 ? 0 : unspecified_bool;
		}

#elif (defined(__MWERKS__) && BOOST_WORKAROUND(__MWERKS__, < 0x3200)) || \
    (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ < 304)) ||    \
    (defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, <= 0x590))

		typedef element_type* (this_type::*unspecified_bool_type)() const;

		operator unspecified_bool_type() const BOOST_NOEXCEPT
		{
			return _shadow == 0 ? 0 : &this_type::get;
		}

#else

		typedef element_type* this_type::*unspecified_bool_type;

		operator unspecified_bool_type() const BOOST_NOEXCEPT
		{
			return _shadow == 0 ? 0 : &this_type::_shadow;
		}
#endif
		// operator! is redundant, but some compilers need it
		bool operator!() const BOOST_NOEXCEPT
		{
			assert(_shadow == boost::intrusive_ptr<element_type>::get()); //px //

			assert(internal_integrity());
			return _shadow == nullptr; //px == 0;
		}

		//		inline operator bool() const { return _shadow != nullptr; }
		//
		//		void close_request()
		//		{
		////#ifdef TEST
		//			_renter->_close_request(_renter);
		////#endif                                     //TEST
		//			_renter = nullptr; //
		//		}

		//	std::shared_ptr<borrower<element_type>> connect() const
		//	{
		//		auto borrower_ = std::make_shared<borrower<element_type>>(this);
		//		_close_request->connect(std::bind(&borrower<element_type>::on_close_requested, borrower_, std::placeholders::_1));
		//		//	QObject::connect(renter_, &can_rent::close_request, [=](QObject* renter_) { borrower_->on_close_request(renter_); });
		//		return borrower_;
		//	}

		void swap(intrusive_ptr& rhs) BOOST_NOEXCEPT
		{

			//			std::mutex m;
			//			std::lock_guard<std::mutex> lock(m);
			boost::intrusive_ptr<element_type>::swap(rhs);
			if (!(!_shadow && !rhs._shadow)) {
				element_type* tmp = _shadow;
				_shadow = rhs._shadow; //boost::intrusive_ptr<element_type>::get();
				rhs._shadow = tmp;     //rhs.boost::intrusive_ptr<element_type>::get(); //
			}

			auto rhs_px =    //rhs._shadow; //
			    rhs._shadow; //boost::template intrusive_ptr<element_type>::get();
			if (rhs_px) {
				rhs.close_receive(                                                             //slot_type(
				    std::bind(&intrusive_ptr::on_close_requested, &rhs, std::placeholders::_1) //, //).track(&rhs)
													       //				    true
				    );
				//                                rhs._close_request_connection = rhs_px->_close_request.connect(std::bind(&intrusive_ptr::on_close_requested, &rhs, std::placeholders::_1));
				rhs.destroy_receive(                                                             //slot_type(
				    std::bind(&intrusive_ptr::on_destroy_requested, &rhs, std::placeholders::_1) //).track(&rhs)
				    );
				//                                rhs._destroy_request_connection = rhs_px->_destroy_request.connect(std::bind(&intrusive_ptr::on_destroy_requested, &rhs, std::placeholders::_1));
			}
			assert(rhs.internal_integrity());
			auto px =          //this->_shadow; //
			    this->_shadow; //boost::template intrusive_ptr<element_type>::get();
			if (px) {
				close_receive(                                                                 //slot_type(
				    std::bind(&intrusive_ptr::on_close_requested, this, std::placeholders::_1) //, //).track(this)
													       //				    true
				    );
				destroy_receive(                                                                 //slot_type(
				    std::bind(&intrusive_ptr::on_destroy_requested, this, std::placeholders::_1) //).track(this)
				    );
			}
			assert(internal_integrity());
		}

		// Have return in pointer, if you don't care nullptr return
		template <typename return_type, typename... arguments>
		return_type* operator()(return_type (element_type::*function_)(arguments...), arguments&&... args)
		{
			if (nullptr != _shadow)
				return (static_cast<element_type*>(_shadow)->*function_)(std::forward<arguments>(args)...);
			else
				return nullptr;
		}
		template <typename return_type, typename... arguments>
		return_type* operator()(return_type (element_type::*function_)(arguments...) const, arguments&&... args) const
		{
			if (nullptr != _shadow)
				return (static_cast<element_type*>(_shadow)->*function_)(std::forward<arguments>(args)...);
			else
				return nullptr;
		}
		//		template <typename return_type, typename... arguments>
		//		return_type* operator()(return_type (element_type::*function_)(const arguments&...), const arguments&&... args)
		//		{
		//			if (nullptr != _shadow)
		//				return (static_cast<element_type*>(_shadow)->*function_)(std::forward<const arguments&>(args)...);
		//			else
		//				return nullptr;
		//		}
		//		template <typename return_type, typename... arguments>
		//		return_type* operator()(return_type (element_type::*function_)(const arguments&...) const, const arguments&&... args) const
		//		{
		//			if (nullptr != _shadow)
		//				return (static_cast<element_type*>(_shadow)->*function_)(std::forward<const arguments&>(args)...);
		//			else
		//				return nullptr;
		//		}
		// have return in value
		template <typename return_type, typename... arguments>
		void operator()(return_type& return_, return_type (element_type::*function_)(arguments...), arguments&&... args)
		{
			if (nullptr != _shadow) return_ = (static_cast<element_type*>(_shadow)->*function_)(std::forward<arguments>(args)...);
		}

		template <typename return_type, typename... arguments>
		void operator()(return_type& return_, return_type (element_type::*function_)(arguments...) const, arguments&&... args) const
		{
			if (nullptr != _shadow) return_ = (static_cast<element_type*>(_shadow)->*function_)(std::forward<arguments>(args)...);
		}


		//		template <typename return_type, typename... arguments>
		//		void operator()(return_type& return_, return_type (element_type::*function_)(const arguments&...), const arguments&&... args)
		//		{
		//			if (nullptr != _shadow)
		//				return_ = (static_cast<element_type*>(_shadow)->*function_)(std::forward<const arguments&>(args)...);
		//		}

		//		template <typename return_type, typename... arguments>
		//		void operator()(return_type& return_, return_type (element_type::*function_)(const arguments&...) const, const arguments&&... args) const
		//		{
		//			if (nullptr != _shadow)
		//				return_ = (static_cast<element_type*>(_shadow)->*function_)(std::forward<const arguments&>(args)...);
		//		}

		//void return
		template <typename return_type, typename... arguments>
		void operator()(return_type (element_type::*function_)(arguments...), arguments&&... args)
		{
			if (nullptr != _shadow) (static_cast<element_type*>(_shadow)->*function_)(std::forward<arguments>(args)...);
		}

		template <typename return_type, typename... arguments>
		void operator()(return_type (element_type::*function_)(arguments...) const, arguments&&... args) const
		{
			if (nullptr != _shadow) (static_cast<element_type*>(_shadow)->*function_)(std::forward<arguments>(args)...);
		}

		//		template <typename return_type, typename... arguments>
		//		void operator()(return_type (element_type::*function_)(const arguments&...), const arguments&&... args)
		//		{
		//			if (nullptr != _shadow)
		//				(static_cast<element_type*>(_shadow)->*function_)(std::forward<const arguments&>(args)...);
		//		}

		//		template <typename return_type, typename... arguments>
		//		void operator()(return_type (element_type::*function_)(const arguments&...) const, const arguments&&... args) const
		//		{
		//			if (nullptr != _shadow)
		//				(static_cast<element_type*>(_shadow)->*function_)(std::forward<const arguments&>(args)...);
		//		}
		template <class T, class U>
		friend void renew_connection(intrusive_ptr<T>& left, const intrusive_ptr<U>& right);
	};

	//        template <typename element_type, typename... args>
	//        inline intrusive_ptr<element_type>
	//        make_intrusive(args&&... args_) { return intrusive_ptr<element_type>(std::forward<std::shared_ptr<element_type>>(std::make_shared<element_type>(std::forward<args>(args_)...))); };

	template <typename element_type, typename... args> //, template <typename> class renter_t
	inline sd::intrusive_ptr<element_type>             //, renter_t
	    make_intrusive(args... args_) = delete;
	//	{
	//		return //sd::intrusive_ptr<element_type>(new element_type(std::forward<args>(args_)...), true); //
	////		    std::forward<sd::intrusive_ptr<element_type>>
	//		    //		    std::move
	//		    (//sd::intrusive_ptr<element_type>(
	//			 new element_type(//std::forward<
	//								      //typename std::remove_reference<args>::type
	//								      //>
	//								      (args_)...)//, true//)
	//		     ); //std::forward<sd::intrusive_ptr<element_type>>(sd::intrusive_ptr<element_type>(new element_type(std::forward<args>(std::remove_const<args>(args_))...), true));
	//	}

	template <class T, class U>
	void renew_connection(intrusive_ptr<T>& left, intrusive_ptr<U>& right)
	{
		(void)right;
		auto px = // left._shadow; //
		    right.boost::template intrusive_ptr<T>::get();
		if (px) {

			right.close_receive(                                                               //slot_type(
			    std::bind(&intrusive_ptr<T>::on_close_requested, &left, std::placeholders::_1) //).track(&left)
			    );
			right.destroy_receive(                                                               //slot_type(
			    std::bind(&intrusive_ptr<T>::on_destroy_requested, &left, std::placeholders::_1) //).track(&left)
			    );

			//                        left._close_request_connection = px->_close_request.connect(std::bind(&intrusive_ptr<T>::on_close_requested, &left, std::placeholders::_1));
			//                        left._destroy_request_connection = px->_destroy_request.connect(std::bind(&intrusive_ptr<T>::on_destroy_requested, &left, std::placeholders::_1));
		}
		//		auto rhs_px = right.boost::template intrusive_ptr<U>::get();
		//		if (rhs_px) {
		//			if (right._close_request_connection.connected())
		//		left._close_request_connection = px->_close_request.connect(std::bind(&intrusive_ptr<T>::on_close_requested, &left, std::placeholders::_1));
		//			if (right._destroy_request_connection.connected())
		//		left._destroy_request_connection = px->_destroy_request.connect(std::bind(&intrusive_ptr<T>::on_destroy_requested, &left, std::placeholders::_1));
		//		}
	}

	template <class T, class U>
	inline bool operator==(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b)
	{
		return a.get() == b.get();
	}

	template <class T, class U>
	inline bool operator!=(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b)
	{
		return a.get() != b.get();
	}

	template <class T, class U>
	inline bool operator==(intrusive_ptr<T> const& a, U* b)
	{
		return a.get() == b;
	}

	template <class T, class U>
	inline bool operator!=(intrusive_ptr<T> const& a, U* b)
	{
		return a.get() != b;
	}

	template <class T, class U>
	inline bool operator==(T* a, intrusive_ptr<U> const& b)
	{
		return a == b.get();
	}

	template <class T, class U>
	inline bool operator!=(T* a, intrusive_ptr<U> const& b)
	{
		return a != b.get();
	}

//
//	template <typename element_type>
//	std::shared_ptr<borrower<element_type>> element_type_constructin(element_type* renter_, std::shared_ptr<boost::signals2::signal<void(element_type*)>> close_request)
//	{
//		//	BOOST_MPL_ASSERT_RELATION((sd::equal_type<typename element_type::super, can_rent>::value), ==, true);
//		//	static_assert(sd::equal_type<typename element_type::super, can_rent>::value == true, "borrowed_type is not a object which can be rent out");


//		auto borrower_ = std::make_shared<borrower<element_type>>(renter_);
//		close_request->connect(std::bind(&borrower<element_type>::on_close_requested, borrower_, std::placeholders::_1));
//		//	QObject::connect(renter_, &can_rent::close_request, [=](QObject* renter_) { borrower_->on_close_request(renter_); });
//		return borrower_;
//	}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

	// Resolve the ambiguity between our op!= and the one in rel_ops

	template <class T>
	inline bool operator!=(intrusive_ptr<T> const& a, intrusive_ptr<T> const& b)
	{
		return a.get() != b.get();
	}

#endif

#if !defined(BOOST_NO_CXX11_NULLPTR)

	template <class T>
	inline bool operator==(intrusive_ptr<T> const& p, boost::detail::sp_nullptr_t) BOOST_NOEXCEPT
	{
		return p.get() == 0;
	}

	template <class T>
	inline bool operator==(boost::detail::sp_nullptr_t, intrusive_ptr<T> const& p) BOOST_NOEXCEPT
	{
		return p.get() == 0;
	}

	template <class T>
	inline bool operator!=(intrusive_ptr<T> const& p, boost::detail::sp_nullptr_t) BOOST_NOEXCEPT
	{
		return p.get() != 0;
	}

	template <class T>
	inline bool operator!=(boost::detail::sp_nullptr_t, intrusive_ptr<T> const& p) BOOST_NOEXCEPT
	{
		return p.get() != 0;
	}

#endif

	template <class T>
	inline bool operator<(intrusive_ptr<T> const& a, intrusive_ptr<T> const& b)
	{
		return std::less<T*>()(a.get(), b.get());
	}

	template <class T>
	void swap(intrusive_ptr<T>& lhs, intrusive_ptr<T>& rhs)
	{
		lhs.swap(rhs);
	}

	// mem_fn support

	template <class T>
	T* get_pointer(intrusive_ptr<T> const& p)
	{
		return p.get();
	}

	template <class T, class U>
	intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const& p)
	{
		return static_cast<T*>(p.get());
	}

	template <class T, class U>
	intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const& p)
	{
		return const_cast<T*>(p.get());
	}

	template <class T, class U>
	intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const& p)
	{
		return dynamic_cast<T*>(p.get());
	}

// operator<<

#if !defined(BOOST_NO_IOSTREAM)

#if defined(BOOST_NO_TEMPLATED_IOSTREAMS) || (defined(__GNUC__) && (__GNUC__ < 3))

	template <class Y>
	std::ostream& operator<<(std::ostream& os, intrusive_ptr<Y> const& p)
	{
		os << p.get();
		return os;
	}

#else

// in STLport's no-iostreams mode no iostream symbols can be used
#ifndef _STLP_NO_IOSTREAMS

#if defined(BOOST_MSVC) && BOOST_WORKAROUND(BOOST_MSVC, < 1300 && __SGI_STL_PORT)
	// MSVC6 has problems finding std::basic_ostream through the using declaration in namespace _STL
	using std::basic_ostream;
	template <class E, class T, class Y>
	basic_ostream<E, T>& operator<<(basic_ostream<E, T>& os, intrusive_ptr<Y> const& p)
#else
	template <class E, class T, class Y>
	std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T>& os, intrusive_ptr<Y> const& p)
#endif
	{
		os << p.get();
		return os;
	}

#endif // _STLP_NO_IOSTREAMS

#endif // __GNUC__ < 3

#endif // !defined(BOOST_NO_IOSTREAM)

	// hash_value

	template <class T>
	struct hash;

	template <class T>
	std::size_t hash_value(boost::intrusive_ptr<T> const& p)
	{
		return boost::hash<T*>()(p.get());
	}

	// namespace boost

	//	template<typename real_renter>
	//	class borrowed_ptr: public std::shared_ptr<real_renter>{

	//	};
}

#endif // LEASE_H
