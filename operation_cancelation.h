#ifndef OPERATION_CANCELATION_H
#define OPERATION_CANCELATION_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "asio.hpp"
#include <map>

struct operation_cancelation
{
   typedef boost::function<void (const asio::error_code&, std::size_t)> function_t;

   struct shared_object
   {
      typedef boost::function<void ()> on_called_t;

      shared_object(function_t const& f, on_called_t const& on_called)
         : f(f)
         , on_called(on_called)
      {}

      void close()
      {
         f = function_t();
      }

      void operator()(const asio::error_code& ec, std::size_t sz) const
      {
         if (f)
         {
            f(ec, sz);
            on_called();
         }
         else
         {
            if (ec != asio::error::operation_aborted)
            {
               for (;;);
            }
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

   function_t wrap(function_t const& f)
   {
      boost::shared_ptr<shared_object> so = boost::make_shared<shared_object>(f, boost::bind(&operation_cancelation::delete_so, this, last_id_));
      somap.insert(somap_t::value_type(last_id_, so));
      ++last_id_;

      return boost::bind(&shared_object::operator(), so, _1, _2);
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

#endif // OPERATION_CANCELATION_H
