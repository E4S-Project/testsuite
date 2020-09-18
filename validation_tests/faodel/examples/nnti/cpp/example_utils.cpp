// Copyright 2018 National Technology & Engineering Solutions of Sandia, 
// LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS,  
// the U.S. Government retains certain rights in this software. 


//#include "nnti/nnti_pch.hpp"

#include "nnti/nntiConfig.h"

#include <unistd.h>
#include <glob.h>
#include <string.h>
#include <pthread.h>
#include <zlib.h>

#include <assert.h>

#include <iostream>
#include <sstream>
#include <thread>

#include <chrono>

#include "nnti/nnti_logger.hpp"

#include "nnti/nnti_util.hpp"

#include "nnti/nnti_transport.hpp"
#include "nnti/nnti_buffer.hpp"
#include "nnti/nnti_wid.hpp"
#include "nnti/transport_factory.hpp"

#include "example_utils.hpp"


NNTI_result_t
cb_func(NNTI_event_t *event, void *context)
{
    std::cout << "This is a callback function.  My parameters are event(" << (void*)event << ") and context(" << (void*)context << ")" << std::endl;
    return NNTI_EIO;
}

NNTI_result_t
find_server_urls(int num_servers,
                 char *my_url,
                 char server_url[][NNTI_URL_LEN],
                 bool &i_am_server)
{
    char *ompi_world_rank = getenv("OMPI_COMM_WORLD_RANK");
    char *slurm_procid = getenv("SLURM_PROCID");

    uint32_t my_rank;
    std::stringstream ss;

    if (ompi_world_rank != nullptr) {
        // this job was launched by Open MPI mpirun.
        my_rank = nnti::util::str2uint32(ompi_world_rank);
    } else if (slurm_procid != nullptr) {
        // this job was launched by srun.
        my_rank = nnti::util::str2uint32(slurm_procid);
    }
    if (my_rank < num_servers) {
        char tmp_filename[1024];
        char filename[1024];
        sprintf(tmp_filename, "tmp_rank%08u_url", my_rank);
        sprintf(filename, "rank%08u_url", my_rank);
        std::ofstream out(tmp_filename);
        out << my_url;
        out.close();
        rename(tmp_filename, filename);

        i_am_server = true;
    } else {
        i_am_server = false;
    }

    sync();

    // loop until all servers have written a URL
    glob_t glob_result;
    glob_result.gl_pathc = 0;
    do {
        int rc = glob("rank*_url", 0, NULL , &glob_result);
        if (rc != 0) {
            log_error("test_utils", "glob failed (rc=%d).  trying to recover by syncing the filesystem.", rc);
            sync();
            nnti::util::sleep(100);
        }
        log_debug("test_utils", "found %d url files", glob_result.gl_pathc);
    } while (glob_result.gl_pathc < num_servers);

    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        std::ifstream in(glob_result.gl_pathv[i]);
        in.read(server_url[i], NNTI_URL_LEN);
        server_url[i][in.gcount()]='\0';
        in.close();
    }
    globfree(&glob_result);

    return NNTI_OK;
}

NNTI_result_t
send_target_hdl(nnti::transports::transport *t,
                NNTI_buffer_t                send_hdl,
                char                        *send_base,
                uint64_t                     send_size,
                NNTI_buffer_t                target_hdl,
                NNTI_peer_t                  peer_hdl,
                NNTI_event_queue_t           eq)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_event_t        event;
    NNTI_event_t        result_event;
    uint32_t            which;
    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    nnti::datatype::nnti_event_callback func_cb(t, cb_func);
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    uint64_t packed_size;
    rc = t->dt_sizeof((void*)target_hdl, &packed_size);
    if (rc != NNTI_OK) {
        log_fatal("test_utils", "dt_sizeof() failed: %d", rc);
    }
    rc = t->dt_pack((void*)target_hdl, send_base, send_size);
    if (rc != NNTI_OK) {
        log_fatal("test_utils", "dt_pack() failed: %d", rc);
    }

    base_wr.op            = NNTI_OP_SEND;
    base_wr.flags         = NNTI_OF_LOCAL_EVENT;
    base_wr.trans_hdl     = nnti::transports::transport::to_hdl(t);
    base_wr.peer          = peer_hdl;
    base_wr.local_hdl     = send_hdl;
    base_wr.local_offset  = 0;
    base_wr.remote_hdl    = NNTI_INVALID_HANDLE;
    base_wr.remote_offset = 0;
    base_wr.length        = packed_size;

    nnti::datatype::nnti_work_request wr(t, base_wr, func_cb);
    NNTI_work_id_t                    wid;

    rc = t->send(&wr, &wid);
    if (rc != NNTI_OK) {
        log_error("test_utils", "send() failed: %d", rc);
        goto cleanup;
    }
    rc = t->eq_wait(&eq, 1, 1000, &which, &event);
    if (rc != NNTI_OK) {
        log_error("test_utils", "eq_wait() failed: %d", rc);
        goto cleanup;
    }

cleanup:
    return rc;
}

NNTI_result_t
recv_target_hdl(nnti::transports::transport *t,
                NNTI_buffer_t                recv_hdl,
                char                        *recv_base,
                NNTI_buffer_t               *target_hdl,
                NNTI_peer_t                 *peer_hdl,
                NNTI_event_queue_t           eq)
{
    NNTI_result_t rc = NNTI_OK;

    NNTI_event_t        event;
    NNTI_event_t        result_event;
    uint32_t            which;
    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    uint32_t msgs_received=0;
    while (true) {
        rc = t->eq_wait(&eq, 1, 1000, &which, &event);
        if (rc != NNTI_OK) {
            log_error("test_utils", "eq_wait() failed: %d", rc);
            continue;
        }
        log_debug_stream("test_utils") << event;
        uint64_t dst_offset = msgs_received * 320;
        rc =  t->next_unexpected(recv_hdl, dst_offset, &result_event);
        if (rc != NNTI_OK) {
            log_error("test_utils", "next_unexpected() failed: %d", rc);
        }
        if (++msgs_received == 1) {
            break;
        }
    }

    // create an nnti_buffer from a packed buffer sent from the client
    t->dt_unpack((void*)target_hdl, recv_base, event.length);

    *peer_hdl   = event.peer;

cleanup:
    return rc;
}

NNTI_result_t
send_hdl(nnti::transports::transport *t,
         NNTI_buffer_t                hdl,
         char                        *hdl_base,
         uint32_t                     hdl_size,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_event_t        event;
    NNTI_event_t        result_event;
    uint32_t            which;
    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    log_debug("test_utils", "send_hdl - enter");

    nnti::datatype::nnti_event_callback null_cb(t);
    nnti::datatype::nnti_event_callback func_cb(t, cb_func);
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    uint64_t packed_size;
    rc = t->dt_sizeof((void*)hdl, &packed_size);
    if (rc != NNTI_OK) {
        log_fatal("test_utils", "dt_sizeof() failed: %d", rc);
    }
    rc = t->dt_pack((void*)hdl, hdl_base, hdl_size);
    if (rc != NNTI_OK) {
        log_fatal("test_utils", "dt_pack() failed: %d", rc);
    }

    base_wr.op            = NNTI_OP_SEND;
    base_wr.flags         = NNTI_OF_LOCAL_EVENT;
    base_wr.trans_hdl     = nnti::transports::transport::to_hdl(t);
    base_wr.peer          = peer_hdl;
    base_wr.local_hdl     = hdl;
    base_wr.local_offset  = 0;
    base_wr.remote_hdl    = NNTI_INVALID_HANDLE;
    base_wr.remote_offset = 0;
    base_wr.length        = packed_size;

    nnti::datatype::nnti_work_request wr(t, base_wr, func_cb);
    NNTI_work_id_t                    wid;

    rc = t->send(&wr, &wid);
    if (rc != NNTI_OK) {
        log_error("test_utils", "send() failed: %d", rc);
        goto cleanup;
    }
    rc = t->eq_wait(&eq, 1, 1000, &which, &event);
    if (rc != NNTI_OK) {
        log_error("test_utils", "eq_wait() failed: %d", rc);
        goto cleanup;
    }

cleanup:
    log_debug("test_utils", "send_hdl - exit");

    return rc;
}

NNTI_result_t
recv_hdl(nnti::transports::transport *t,
         NNTI_buffer_t                recv_hdl,
         char                        *recv_base,
         uint32_t                     recv_size,
         NNTI_buffer_t               *hdl,
         NNTI_peer_t                 *peer_hdl,
         NNTI_event_queue_t           eq)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_event_t        event;
    NNTI_event_t        result_event;
    uint32_t            which;
    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    log_debug("test_utils", "recv_hdl - enter");

    uint32_t msgs_received=0;
    while (true) {
        rc = t->eq_wait(&eq, 1, 1000, &which, &event);
        if (rc != NNTI_OK) {
            log_error("test_utils", "eq_wait() failed: %d", rc);
            continue;
        }
        log_debug_stream("test_utils") << event;
        uint64_t dst_offset = msgs_received * 320;
        rc =  t->next_unexpected(recv_hdl, dst_offset, &result_event);
        if (rc != NNTI_OK) {
            log_error("test_utils", "next_unexpected() failed: %d", rc);
        }
        if (++msgs_received == 1) {
            break;
        }
    }

    log_debug("test_utils", "handle received");

    // create an nnti_buffer from a packed buffer sent from the client
    t->dt_unpack(hdl, recv_base, event.length);

    *peer_hdl   = event.peer;

cleanup:
    log_debug("test_utils", "recv_hdl - exit");

    return rc;
}

NNTI_result_t
send_ack(nnti::transports::transport *t,
         NNTI_buffer_t                hdl,
         NNTI_buffer_t                ack_hdl,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_event_t        event;
    NNTI_event_t        result_event;
    uint32_t            which;
    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    log_debug("test_utils", "send_ack - enter");

    base_wr.op            = NNTI_OP_SEND;
    base_wr.flags         = NNTI_OF_LOCAL_EVENT;
    base_wr.trans_hdl     = nnti::transports::transport::to_hdl(t);
    base_wr.peer          = peer_hdl;
    base_wr.local_hdl     = hdl;
    base_wr.local_offset  = 0;
    base_wr.remote_hdl    = ack_hdl;
    base_wr.remote_offset = 0;
    base_wr.length        = 64;

    nnti::datatype::nnti_work_request wr(t, base_wr);
    NNTI_work_id_t                    wid;

    rc = t->send(&wr, &wid);
    if (rc != NNTI_OK) {
        log_error("test_utils", "send() failed: %d", rc);
        goto cleanup;
    }
    rc = t->eq_wait(&eq, 1, 1000, &which, &event);
    if (rc != NNTI_OK) {
        log_error("test_utils", "eq_wait() failed: %d", rc);
        goto cleanup;
    }

cleanup:
    log_debug("test_utils", "send_ack - exit");

    return rc;
}

NNTI_result_t
recv_ack(nnti::transports::transport *t,
         NNTI_buffer_t                ack_hdl,
         NNTI_peer_t                 *peer_hdl,
         NNTI_event_queue_t           eq)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_event_t        event;
    NNTI_event_t        result_event;
    uint32_t            which;
    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    log_debug("test_utils", "recv_ack - enter");

    uint32_t msgs_received=0;
    while (true) {
        rc = t->eq_wait(&eq, 1, 1000, &which, &event);
        if (rc != NNTI_OK) {
            log_error("test_utils", "eq_wait() failed: %d", rc);
            continue;
        }
        log_debug_stream("test_utils") << event;
        if (++msgs_received == 1) {
            break;
        }
    }

    *peer_hdl   = event.peer;

cleanup:
    log_debug("test_utils", "recv_ack - exit");

    return rc;
}

NNTI_result_t
populate_buffer(nnti::transports::transport *t,
                uint32_t                     seed,
                uint64_t                     offset_multiplier,
                NNTI_buffer_t                buf_hdl,
                char                        *buf_base,
                uint64_t                     buf_size)
{
    NNTI_result_t rc = NNTI_OK;

    char packed[312];
    int  packed_size = 312;

    rc = t->dt_pack((void*)buf_hdl, packed, 312);
    if (rc != NNTI_OK) {
        log_fatal("test_utils", "dt_pack() failed: %d", rc);
    }

    char *payload = buf_base + ((packed_size+4+4)*offset_multiplier);

    log_debug("test_utils", "buf_base=%p buf_size=%lu offset_multiplier=%lu offset=%lu", buf_base, buf_size, offset_multiplier, ((packed_size+4+4)*offset_multiplier));

    memcpy(payload+8, packed, packed_size);  // the data
    *(uint32_t*)(payload+4) = seed;          // the salt

    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, ((Bytef*)payload)+4, 316); // the checksum
    *(uint32_t*)payload = 0;
    *(uint32_t*)payload = crc;

    log_debug("test_utils", "seed=0x%x  payload=%p  payload[0]=%08x  crc=%08x", seed, payload, *(uint32_t*)payload, crc);

    return NNTI_OK;
}

bool
verify_buffer(char          *buf_base,
              uint64_t       buf_offset,
              uint64_t       buf_size)
{
    bool success=true;

    char     *payload = buf_base;

    payload = buf_base + buf_offset;
    uint32_t seed = *(uint32_t*)(payload+4); // the salt

    uLong crc = crc32(0L, Z_NULL, 0);
    crc = crc32(crc, ((Bytef*)payload)+4, 316); // the checksum

    log_debug("test_utils", "seed=0x%x  payload[0]=0x%08x  crc=0x%08x", seed, *(uint32_t*)payload, crc);

    if (*(uint32_t*)payload != crc) {
        log_error("test_utils", "crc mismatch (expected=0x%08x  actual=0x%08x)", *(uint32_t*)payload, (uint32_t)crc);
        success = false;
    }

    return success;
}

NNTI_result_t
wait_data(nnti::transports::transport *t,
          NNTI_event_queue_t           eq)
{
    NNTI_result_t rc = NNTI_OK;

    NNTI_event_t        event;
    uint32_t            which;

    rc = t->eq_wait(&eq, 1, 1000, &which, &event);
    if (rc != NNTI_OK) {
        log_error("test_utils", "eq_wait() failed: %d", rc);
    }

    return rc;
}

NNTI_result_t
send_data_async(nnti::transports::transport         *t,
                uint64_t                             offset_multiplier,
                NNTI_buffer_t                        src_hdl,
                NNTI_buffer_t                        dst_hdl,
                NNTI_peer_t                          peer_hdl,
                nnti::datatype::nnti_event_callback &cb,
                void                                *context)
{
    NNTI_result_t rc = NNTI_OK;

    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    uint32_t msg_size = 320;

    base_wr.op            = NNTI_OP_SEND;
    base_wr.flags         = NNTI_OF_LOCAL_EVENT;
    base_wr.trans_hdl     = nnti::transports::transport::to_hdl(t);
    base_wr.peer          = peer_hdl;
    base_wr.local_hdl     = src_hdl;
    base_wr.local_offset  = offset_multiplier * msg_size;
    base_wr.remote_hdl    = dst_hdl;
    base_wr.remote_offset = offset_multiplier * msg_size;
    base_wr.length        = msg_size;
    base_wr.cb_context    = context;

    nnti::datatype::nnti_work_request wr(t, base_wr, cb);
    NNTI_work_id_t                    wid;

    rc = t->send(&wr, &wid);
    if (rc != NNTI_OK) {
        log_error("test_utils", "send() failed: %d", rc);
        goto cleanup;
    }

cleanup:
    return rc;
}

NNTI_result_t
send_data_async(nnti::transports::transport *t,
                uint64_t                     offset_multiplier,
                NNTI_buffer_t                src_hdl,
                NNTI_buffer_t                dst_hdl,
                NNTI_peer_t                  peer_hdl)
{
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    return send_data_async(
        t,
        offset_multiplier,
        src_hdl,
        dst_hdl,
        peer_hdl,
        obj_cb,
        nullptr);
}

NNTI_result_t
send_data(nnti::transports::transport          *t,
          uint64_t                             offset_multiplier,
          NNTI_buffer_t                        src_hdl,
          NNTI_buffer_t                        dst_hdl,
          NNTI_peer_t                          peer_hdl,
          NNTI_event_queue_t                   eq,
          nnti::datatype::nnti_event_callback &cb,
          void                                *context)
{
    NNTI_result_t rc=NNTI_OK;

    rc = send_data_async(t,
                   offset_multiplier,
                   src_hdl,
                   dst_hdl,
                   peer_hdl,
                   cb,
                   context);

    rc = wait_data(t, eq);

cleanup:
    return rc;
}

NNTI_result_t
send_data(nnti::transports::transport *t,
          uint64_t                     offset_multiplier,
          NNTI_buffer_t                src_hdl,
          NNTI_buffer_t                dst_hdl,
          NNTI_peer_t                  peer_hdl,
          NNTI_event_queue_t           eq)
{
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    return send_data(
        t,
        offset_multiplier,
        src_hdl,
        dst_hdl,
        peer_hdl,
        eq,
        obj_cb,
        nullptr);
}

NNTI_result_t
recv_data(nnti::transports::transport *t,
          NNTI_event_queue_t           eq,
          NNTI_event_t                *event)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_event_t        result_event;
    uint32_t            which;
    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    uint32_t msgs_received=0;
    while (true) {
        rc = t->eq_wait(&eq, 1, 1000, &which, event);
        if (rc != NNTI_OK) {
            log_error("test_utils", "eq_wait() failed: %d", rc);
            continue;
        }
        log_debug_stream("test_utils") << event;

        if (++msgs_received == 1) {
            break;
        }
    }

cleanup:
    return rc;
}

NNTI_result_t
get_data_async(nnti::transports::transport         *t,
               NNTI_buffer_t                        src_hdl,
               NNTI_buffer_t                        dst_hdl,
               NNTI_peer_t                          peer_hdl,
               nnti::datatype::nnti_event_callback &cb,
               void                                *context)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    log_debug("test_utils", "get_data_async - enter");

    base_wr.op            = NNTI_OP_GET;
    base_wr.flags         = NNTI_OF_LOCAL_EVENT;
    base_wr.trans_hdl     = nnti::transports::transport::to_hdl(t);
    base_wr.peer          = peer_hdl;
    base_wr.local_hdl     = dst_hdl;
    base_wr.local_offset  = 0;
    base_wr.remote_hdl    = src_hdl;
    base_wr.remote_offset = 0;
    base_wr.length        = 3200;
    base_wr.cb_context    = context;

    nnti::datatype::nnti_work_request wr(t, base_wr, cb);
    NNTI_work_id_t                    wid;

    rc = t->get(&wr, &wid);
    if (rc != NNTI_OK) {
        log_error("test_utils", "send() failed: %d", rc);
    }

    log_debug("test_utils", "get_data_async - exit");

    return rc;
}

NNTI_result_t
get_data_async(nnti::transports::transport *t,
               NNTI_buffer_t                src_hdl,
               NNTI_buffer_t                dst_hdl,
               NNTI_peer_t                  peer_hdl)
{
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    return get_data_async(t,
                          src_hdl,
                          dst_hdl,
                          peer_hdl,
                          obj_cb,
                          nullptr);
}

NNTI_result_t
get_data(nnti::transports::transport         *t,
         NNTI_buffer_t                        src_hdl,
         NNTI_buffer_t                        dst_hdl,
         NNTI_peer_t                          peer_hdl,
         NNTI_event_queue_t                   eq,
         nnti::datatype::nnti_event_callback &cb,
         void                                *context)
{
    NNTI_result_t rc=NNTI_OK;

    log_debug("test_utils", "get_data - enter");

    rc = get_data_async(t,
                        src_hdl,
                        dst_hdl,
                        peer_hdl,
                        cb,
                        context);

    rc = wait_data(t, eq);

    log_debug("test_utils", "get_data - exit");

    return rc;
}

NNTI_result_t
get_data(nnti::transports::transport *t,
         NNTI_buffer_t                src_hdl,
         NNTI_buffer_t                dst_hdl,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq)
{
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    return get_data(t,
                    src_hdl,
                    dst_hdl,
                    peer_hdl,
                    eq,
                    obj_cb,
                    nullptr);
}

NNTI_result_t
put_data_async(nnti::transports::transport         *t,
               NNTI_buffer_t                        src_hdl,
               NNTI_buffer_t                        dst_hdl,
               NNTI_peer_t                          peer_hdl,
               nnti::datatype::nnti_event_callback &cb,
               void                                *context)
{
    NNTI_result_t rc=NNTI_OK;

    NNTI_work_request_t base_wr = NNTI_WR_INITIALIZER;

    log_debug("test_utils", "put_data_async - enter");

    base_wr.op            = NNTI_OP_PUT;
    base_wr.flags         = NNTI_OF_LOCAL_EVENT;
    base_wr.trans_hdl     = nnti::transports::transport::to_hdl(t);
    base_wr.peer          = peer_hdl;
    base_wr.local_hdl     = src_hdl;
    base_wr.local_offset  = 0;
    base_wr.remote_hdl    = dst_hdl;
    base_wr.remote_offset = 0;
    base_wr.length        = 3200;
    base_wr.cb_context    = context;

    nnti::datatype::nnti_work_request wr(t, base_wr, cb);
    NNTI_work_id_t                    wid;

    rc = t->put(&wr, &wid);
    if (rc != NNTI_OK) {
        log_error("test_utils", "send() failed: %d", rc);
    }

    log_debug("test_utils", "put_data_async - exit");

    return rc;
}

NNTI_result_t
put_data_async(nnti::transports::transport *t,
               NNTI_buffer_t                src_hdl,
               NNTI_buffer_t                dst_hdl,
               NNTI_peer_t                  peer_hdl)
{
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    return put_data_async(t,
                          src_hdl,
                          dst_hdl,
                          peer_hdl,
                          obj_cb,
                          nullptr);
}

NNTI_result_t
put_data(nnti::transports::transport         *t,
         NNTI_buffer_t                        src_hdl,
         NNTI_buffer_t                        dst_hdl,
         NNTI_peer_t                          peer_hdl,
         NNTI_event_queue_t                   eq,
         nnti::datatype::nnti_event_callback &cb,
         void                                *context)
{
    NNTI_result_t rc=NNTI_OK;

    log_debug("test_utils", "put_data - enter");

    rc = put_data_async(t,
                        src_hdl,
                        dst_hdl,
                        peer_hdl,
                        cb,
                        context);

    rc = wait_data(t, eq);

    log_debug("test_utils", "put_data - exit");

    return rc;
}

NNTI_result_t
put_data(nnti::transports::transport *t,
         NNTI_buffer_t                src_hdl,
         NNTI_buffer_t                dst_hdl,
         NNTI_peer_t                  peer_hdl,
         NNTI_event_queue_t           eq)
{
    nnti::datatype::nnti_event_callback obj_cb(t, callback());

    return put_data(t,
                    src_hdl,
                    dst_hdl,
                    peer_hdl,
                    eq,
                    obj_cb,
                    nullptr);
}
