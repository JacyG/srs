/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 OSSRS(winlin)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SRS_SERVICE_ST_HPP
#define SRS_SERVICE_ST_HPP

#include <srs_core.hpp>

#include <string>
#include <st.h>

#include <srs_protocol_io.hpp>

// initialize st, requires epoll.
extern int srs_st_init();

// close the netfd, and close the underlayer fd.
// @remark when close, user must ensure io completed.
extern void srs_close_stfd(st_netfd_t& stfd);

/**
 * the socket provides TCP socket over st,
 * that is, the sync socket mechanism.
 */
class SrsStSocket : public ISrsProtocolReaderWriter
{
private:
    // The recv/send timeout in ms.
    // @remark Use SRS_CONSTS_NO_TMMS for never timeout in ms.
    int64_t rtm;
    int64_t stm;
    // The recv/send data in bytes
    int64_t rbytes;
    int64_t sbytes;
    // The underlayer st fd.
    st_netfd_t stfd;
public:
    SrsStSocket();
    virtual ~SrsStSocket();
public:
    // Initialize the socket with stfd, user must manage it.
    virtual int initialize(st_netfd_t fd);
public:
    virtual bool is_never_timeout(int64_t tm);
    virtual void set_recv_timeout(int64_t tm);
    virtual int64_t get_recv_timeout();
    virtual void set_send_timeout(int64_t tm);
    virtual int64_t get_send_timeout();
    virtual int64_t get_recv_bytes();
    virtual int64_t get_send_bytes();
public:
    /**
     * @param nread, the actual read bytes, ignore if NULL.
     */
    virtual int read(void* buf, size_t size, ssize_t* nread);
    virtual int read_fully(void* buf, size_t size, ssize_t* nread);
    /**
     * @param nwrite, the actual write bytes, ignore if NULL.
     */
    virtual int write(void* buf, size_t size, ssize_t* nwrite);
    virtual int writev(const iovec *iov, int iov_size, ssize_t* nwrite);
};

/**
 * The client to connect to server over TCP.
 * User must never reuse the client when close it.
 * Usage:
 *      SrsTcpClient client("127.0.0.1", 1935,9000);
 *      client.connect();
 *      client.write("Hello world!", 12, NULL);
 *      client.read(buf, 4096, NULL);
 * @remark User can directly free the object, which will close the fd.
 */
class SrsTcpClient : public ISrsProtocolReaderWriter
{
private:
    st_netfd_t stfd;
    SrsStSocket* io;
private:
    std::string host;
    int port;
    // The timeout in ms.
    int64_t timeout;
public:
    /**
     * Constructor.
     * @param h the ip or hostname of server.
     * @param p the port to connect to.
     * @param tm the timeout in ms.
     */
    SrsTcpClient(std::string h, int p, int64_t tm);
    virtual ~SrsTcpClient();
public:
    /**
     * Connect to server over TCP.
     * @remark We will close the exists connection before do connect.
     */
    virtual int connect();
private:
    /**
     * Close the connection to server.
     * @remark User should never use the client when close it.
     */
    virtual void close();
    // interface ISrsProtocolReaderWriter
public:
    virtual bool is_never_timeout(int64_t tm);
    virtual void set_recv_timeout(int64_t tm);
    virtual int64_t get_recv_timeout();
    virtual void set_send_timeout(int64_t tm);
    virtual int64_t get_send_timeout();
    virtual int64_t get_recv_bytes();
    virtual int64_t get_send_bytes();
    virtual int read(void* buf, size_t size, ssize_t* nread);
    virtual int read_fully(void* buf, size_t size, ssize_t* nread);
    virtual int write(void* buf, size_t size, ssize_t* nwrite);
    virtual int writev(const iovec *iov, int iov_size, ssize_t* nwrite);
};

#endif

