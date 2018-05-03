#include "net/event_dispatcher.h"
#include "net/event_handler.h"
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <sys/poll.h>
#include <vector>

/*
 * I/O复用之POLL模型调度分发
 */
class DispatcherPoll : public EventDispatcher
{
  typedef struct pollfd pollfd_t;
  typedef std::vector<pollfd_t> pollfd_array;
public:
  DispatcherPoll();
  virtual ~DispatcherPoll();

  virtual int get_last_errno(){ return last_errno_;}

  virtual bool add_event(const int handle, const bool readable, const bool writeable);
  virtual bool mod_event(const int handle, const bool readable, const bool writeable);
  virtual bool del_event(const int handle);
  virtual int handle_events(ready_event_t& ready, const int timeout_ms=-1);

private:
  int last_errno_;
  pollfd_array pollfds_;
} ;


DispatcherPoll::DispatcherPoll() 
:last_errno_(0)
,pollfds_()
{
}

DispatcherPoll::~DispatcherPoll()
{
  pollfds_.clear();
}

bool DispatcherPoll::add_event(const int handle, const bool readable, const bool writeable)
{
  pollfd_t pfd;
  pfd.fd=handle;
  pfd.revents=0;
  pfd.events=POLLERR | POLLHUP;

  if (readable)
    pfd.events |= POLLIN | POLLPRI;

  if (writeable)
    pfd.events |= POLLOUT;

  pollfds_.push_back(pfd);

  return true;
}

bool DispatcherPoll::mod_event(const int handle, const bool readable, const bool writeable)
{
  pollfd_t pfd;
  pfd.fd=handle;
  pfd.revents=0;
  pfd.events=POLLERR | POLLHUP;

  if (readable)
    pfd.events |= POLLIN | POLLPRI;

  if (writeable)
    pfd.events |= POLLOUT;

  pollfd_array::iterator it;
  for (it=pollfds_.begin(); it != pollfds_.end(); ++it)
  {
    pollfd_t& pollfd=*it;
    if (handle == pollfd.fd)
    {
      *it=pfd;
      return true;
    }
  }

  assert(0);
  last_errno_=ENOTSOCK;
  return false;
}

bool DispatcherPoll::del_event(const int handle)
{
  pollfd_array::iterator it;
  for (it=pollfds_.begin(); it != pollfds_.end(); ++it)
  {
    pollfd_t& pollfd=*it;
    if (handle == pollfd.fd)
      break;
  }

  if (it != pollfds_.end())
    pollfds_.erase(it);

  return true;
}

int DispatcherPoll::handle_events(ready_event_t& ready, const int timeout_ms)
{
  int ret=::poll(&pollfds_[0], pollfds_.size(), timeout_ms);
  if (ret < 0)
  {
    last_errno_=errno;
    return -1;
  } 
  else if (0 == ret)
  {
    return ret;
  } 
  else
  {
    for (size_t i=0; i < pollfds_.size(); ++i)
    {
      pollfd_t& pollfd=pollfds_[i];

      if (pollfd.revents & (POLLIN | POLLPRI) )
      {
       ready.push_back( std::make_pair<int,int>(pollfd.fd,EventDispatcher::EVENT_READ ) );
      }

      if (pollfd.revents & POLLOUT )
      {
        ready.push_back( std::make_pair<int,int>(pollfd.fd,EventDispatcher::EVENT_WRITE) );
      }

      if (pollfd.revents & (POLLERR | POLLHUP) )
      {
        ready.push_back( std::make_pair<int,int> (pollfd.fd,EventDispatcher::EVENT_ERROR) );
      }
    }
  }

  return ret;
}