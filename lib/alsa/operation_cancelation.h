#ifndef OPERATION_CANCELATION_H
#define OPERATION_CANCELATION_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <map>

struct operation_cancelation
{
   struct shared_object
   {
      virtual ~shared_object() = 0;
      virtual void close() = 0;
   };

   struct shared_object_impl0 : shared_object
   {
      typedef boost::function<void ()> on_called_t;
      typedef boost::function<void ()> function_t;

      shared_object_impl0(function_t const& f, on_called_t const& on_called)
         : f(f)
         , on_called(on_called)
      {}

      void close()
      {
         f = function_t();
      }

      void operator()() const
      {
         if (f)
         {
            f();
            if (f) // double check required, because ~operation_cancelation and shared_object::close can be called from f
               on_called();
         }
      }

   private:
      function_t f;
      on_called_t on_called;
   };

   template <typename T1>
   struct shared_object_impl1 : shared_object
   {
      typedef boost::function<void ()> on_called_t;
      typedef boost::function<void (T1)> function_t;

      shared_object_impl1(function_t const& f, on_called_t const& on_called)
         : f(f)
         , on_called(on_called)
      {}

      void close()
      {
         f = function_t();
      }

      void operator()(T1 ec) const
      {
         if (f)
         {
            f(ec);
            if (f) // double check required, because ~operation_cancelation and shared_object::close can be called from f
               on_called();
         }
      }

   private:
      function_t f;
      on_called_t on_called;
   };

   template <typename T1, typename T2>
   struct shared_object_impl2 : shared_object
   {
      typedef boost::function<void ()> on_called_t;
      typedef boost::function<void (T1, T2)> function_t;

      shared_object_impl2(function_t const& f, on_called_t const& on_called)
         : f(f)
         , on_called(on_called)
      {}

      void close()
      {
         f = function_t();
      }

      void operator()(T1 ec, T2 sz) const
      {
         if (f)
         {
            f(ec, sz);
            if (f) // double check required, because ~operation_cancelation and shared_object::close can be called from f
               on_called();
         }
      }

   private:
      function_t f;
      on_called_t on_called;
   };

   operation_cancelation()
      : last_id_()
   {}

   ~operation_cancelation()
   {
      for (somap_t::const_iterator i = somap.begin(); i != somap.end(); ++i)
         i->second->close();
   }

   boost::function<void ()> wrap(boost::function<void ()> const& f)
   {
      boost::shared_ptr<shared_object_impl0> so = boost::make_shared<shared_object_impl0>(f, boost::bind(&operation_cancelation::delete_so, this, last_id_));
      somap.insert(somap_t::value_type(last_id_, so));
      ++last_id_;

       return boost::bind(&shared_object_impl0::operator(), so);
   }

   template <typename T1>
   boost::function<void (T1)> wrap(boost::function<void (T1)> const& f)
   {
      boost::shared_ptr<shared_object_impl1<T1> > so = boost::make_shared<shared_object_impl1<T1> >(f, boost::bind(&operation_cancelation::delete_so, this, last_id_));
      somap.insert(somap_t::value_type(last_id_, so));
      ++last_id_;

      return boost::bind(&shared_object_impl1<T1>::operator(), so, _1);
   }

   template <typename T1, typename T2>
   boost::function<void (T1, T2)> wrap(boost::function<void (T1, T2)> const& f)
   {
      boost::shared_ptr<shared_object_impl2<T1, T2> > so = boost::make_shared<shared_object_impl2<T1, T2> >(f, boost::bind(&operation_cancelation::delete_so, this, last_id_));
      somap.insert(somap_t::value_type(last_id_, so));
      ++last_id_;

      return boost::bind(&shared_object_impl2<T1, T2>::operator(), so, _1, _2);
   }

   void delete_so(size_t id)
   {
      somap_t::iterator i = somap.find(id);
      if (i != somap.end())
         somap.erase(i);
      else
         assert(false);
   }

   size_t last_id_;
   typedef std::map<size_t, boost::shared_ptr<shared_object> > somap_t;
   somap_t somap;
};

inline operation_cancelation::shared_object::~shared_object()
{}

#endif // OPERATION_CANCELATION_H
