// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 


#include "nnti/nntiConfig.h"

#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <zlib.h>

#include <assert.h>

#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>

#include "faodel-common/Configuration.hh"
#include "faodel-common/Bootstrap.hh"

#include "whookie/Server.hh"

#include "nnti/nnti_logger.hpp"

#include "nnti/nnti_util.hpp"

#include "nnti/nnti_transport.hpp"
#include "nnti/nnti_buffer.hpp"
#include "nnti/nnti_wid.hpp"
#include "nnti/transport_factory.hpp"

#include "example_utils.hpp"

std::string default_config_string = R"EOF(
nnti.transport.name                           mpi
)EOF";

bool success=true;

class pingpong_context {
public:
    nnti::datatype::nnti_event_callback *cb_;
    nnti::transports::transport         *transport_;
    struct buffer_properties             send_src_;
    struct buffer_properties             send_target_;

    std::atomic<bool>                    shutdown_now_;

public:
    pingpong_context(
        nnti::datatype::nnti_event_callback *cb,
        nnti::transports::transport         *transport,
        struct buffer_properties             send_src,
        struct buffer_properties             send_target)
    : cb_(cb),
      transport_(transport),
      send_src_(send_src),
      send_target_(send_target)
    {
        shutdown_now_.store(false);
        return;
    }
};

class pingpong_callback {
public:
    pingpong_callback()
    {
        return;
    }

    NNTI_result_t operator() (NNTI_event_t *event, void *context) {
        NNTI_result_t rc = NNTI_OK;

        pingpong_context *c = (pingpong_context*)context;

        log_debug("pingpong_callback", "enter");

        switch (event->type) {
            case NNTI_EVENT_SEND:
                {
                log_debug("pingpong_callback", "SEND event");
                }
                break;
            case NNTI_EVENT_RECV:
                {
                log_debug("pingpong_callback", "RECV event");

                // issue another send
                if (verify_buffer((char*)event->start, event->offset, event->length) == false) {
                    success = false;
                }

                char     *payload = (char*)event->start + event->offset;
                uint32_t seed     = *(uint32_t*)(payload+4); // the salt
                seed++;

                rc = populate_buffer(c->transport_, seed, 0, c->send_src_.hdl, c->send_src_.base, c->send_src_.size);

                rc = send_data_async(c->transport_, 0, c->send_src_.hdl, c->send_target_.hdl, event->peer, *c->cb_, context);
                }

                break;
            default:
                break;
        }

        if (c->shutdown_now_.load() == true) {
            rc = NNTI_ECANCELED;
        }

        log_debug("pingpong_callback", "exit");

        return rc;
    }
};

void
shutdown_cb(pingpong_context *ppc)
{
    ppc->shutdown_now_.store(true);
}

int main(int argc, char *argv[])
{
    NNTI_result_t rc;
    nnti::transports::transport *t=nullptr;

    int i=0;

    std::stringstream logfile;

    char my_url[NNTI_URL_LEN];
    char server_url[1][NNTI_URL_LEN];

    char  my_hostname[NNTI_HOSTNAME_LEN];
    char *server_hostname=argv[1];

    gethostname(my_hostname, NNTI_HOSTNAME_LEN);

    faodel::Configuration config(default_config_string);
    config.AppendFromReferences();

    faodel::bootstrap::Start(config, whookie::bootstrap);

    t = nnti::transports::factory::get_instance(config);

    nnti::datatype::nnti_event_callback null_cb(t);
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    t->start();
    t->get_url(my_url, NNTI_URL_LEN);

    NNTI_peer_t         peer_hdl;

    NNTI_event_queue_t  unexpected_eq;
    NNTI_event_queue_t  pingpong_eq;
    NNTI_event_t        event;

    struct buffer_properties src_buf;
    struct buffer_properties my_pingpong_buf;
    struct buffer_properties peer_pingpong_buf;

    rc = t->eq_create(128, NNTI_EQF_UNEXPECTED, &unexpected_eq);
    rc = t->eq_create(128, NNTI_EQF_UNSET, &pingpong_eq);

    pingpong_callback *cb = new pingpong_callback();
    nnti::datatype::nnti_event_callback *ppcb = new nnti::datatype::nnti_event_callback(t, *cb);
    pingpong_context                    *ppc  = new pingpong_context(ppcb,
                                                                     t,
                                                                     my_pingpong_buf,
                                                                     peer_pingpong_buf);

    whookie::Server::registerHook("/pingpong/shutdown", [ppc] (const std::map<std::string,std::string> &args, std::stringstream &results){
        shutdown_cb(ppc);
    });

    src_buf.size=3200;
    rc = t->alloc(src_buf.size,
                  (NNTI_buffer_flags_t)(NNTI_BF_LOCAL_READ|NNTI_BF_LOCAL_WRITE|NNTI_BF_REMOTE_READ|NNTI_BF_REMOTE_WRITE),
                  unexpected_eq,
                  null_cb,
                  nullptr,
                  &src_buf.base,
                  &src_buf.hdl);
    my_pingpong_buf.size=3200;
    rc = t->alloc(my_pingpong_buf.size,
                  (NNTI_buffer_flags_t)(NNTI_BF_LOCAL_READ|NNTI_BF_LOCAL_WRITE|NNTI_BF_REMOTE_READ|NNTI_BF_REMOTE_WRITE),
                  pingpong_eq,
                  *ppcb,
                  ppc,
                  &my_pingpong_buf.base,
                  &my_pingpong_buf.hdl);

    ppc->send_src_ = my_pingpong_buf;

    bool i_am_server = false;
    if (server_hostname) {
        sprintf(server_url[0], "ib://%s:1990/", server_hostname);
        log_debug("IBMultiPingPongTest", "my_url=%s  server_url[0]=%s", my_url, server_url[0]);
        i_am_server = !strcmp(server_url[0], my_url);
    } else {
        uint32_t num_servers = 1;
        find_server_urls(num_servers, my_url, server_url, i_am_server);
    }

    if (i_am_server) {
        rc = recv_target_hdl(t, src_buf.hdl, src_buf.base, &peer_pingpong_buf.hdl, &peer_hdl, unexpected_eq);
        if (rc != NNTI_OK) {
            log_error("PingPong", "recv_target_hdl() failed: %d", rc);
        }

        ppc->send_target_ = peer_pingpong_buf;

        rc = send_target_hdl(t, src_buf.hdl, src_buf.base, src_buf.size, my_pingpong_buf.hdl, peer_hdl, unexpected_eq);
        if (rc != NNTI_OK) {
            log_error("PingPong", "send_target_hdl() failed: %d", rc);
        }

        // the state machine will put an event on the eq when the volley is over
        rc = recv_data(t, pingpong_eq, &event);

        // send the all clear message to the other process
        rc = send_target_hdl(t, src_buf.hdl, src_buf.base, src_buf.size, my_pingpong_buf.hdl, peer_hdl, unexpected_eq);
        if (rc != NNTI_OK) {
            log_error("PingPong", "send_target_hdl() failed: %d", rc);
        }
    } else {
        // give the server a chance to startup
        nnti::util::sleep(1000);

        rc = t->connect(server_url[0], 1000, &peer_hdl);

        rc = send_target_hdl(t, src_buf.hdl, src_buf.base, src_buf.size, my_pingpong_buf.hdl, peer_hdl, unexpected_eq);
        if (rc != NNTI_OK) {
            log_error("PingPong", "send_target_hdl() failed: %d", rc);
        }

        NNTI_peer_t recv_peer;
        rc = recv_target_hdl(t, src_buf.hdl, src_buf.base, &peer_pingpong_buf.hdl, &recv_peer, unexpected_eq);
        if (rc != NNTI_OK) {
            log_error("PingPong", "recv_target_hdl() failed: %d", rc);
        }

        ppc->send_target_ = peer_pingpong_buf;

        rc = populate_buffer(t, 0, 0, my_pingpong_buf.hdl, my_pingpong_buf.base, my_pingpong_buf.size);

        rc = send_data_async(t, 0, my_pingpong_buf.hdl, peer_pingpong_buf.hdl, peer_hdl, *ppcb, ppc);

        // the state machine will put an event on the eq when the volley is over
        rc = recv_data(t, pingpong_eq, &event);

        // wait for the all clear message from the other process
        rc = recv_data(t, unexpected_eq, &event);

        t->disconnect(peer_hdl);
    }

    if (t->initialized()) {
        t->stop();
    } else {
        success = false;
    }

    if (success) {
        log_debug_stream("PingPong") << "\nEnd Result: TEST PASSED";
        std::cout << "\nEnd Result: TEST PASSED" << std::endl;
    } else {
        log_debug_stream("PingPong") << "\nEnd Result: TEST FAILED";
        std::cout << "\nEnd Result: TEST FAILED" << std::endl;
    }

    nnti::core::logger::fini();

    return (success ? 0 : 1 );
}
