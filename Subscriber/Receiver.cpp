#include "Receiver.h"

Receiver::Receiver(Decoder &decoder, boost::asio::io_service &io_service,
                   const boost::asio::ip::address &listen_address,
                   const boost::asio::ip::address &multicast_address,
                   const short multicast_port)
    : socket_(io_service), listen_address_(listen_address),
      multicast_address_(multicast_address), multicast_port_(multicast_port),
      decoder_(decoder) {}

void Receiver::Join() {

  boost::asio::ip::udp::endpoint listen_endpoint(boost::asio::ip::udp::v4(),
                                                 multicast_port_);

  socket_.open(listen_endpoint.protocol());
  socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
  socket_.set_option(boost::asio::ip::multicast::enable_loopback(true));

  socket_.bind(listen_endpoint);
  socket_.set_option(
      boost::asio::ip::multicast::join_group(multicast_address_.to_v4()));

  socket_.async_receive_from(
      boost::asio::buffer(data_, max_length), sender_endpoint_,
      boost::bind(&Receiver::HandleReceiveFrom, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

void Receiver::Leave() { socket_.close(); }

void Receiver::HandleReceiveFrom(const boost::system::error_code &error,
                                 size_t received) {

  if (!error) {

    // trigger callbacks registered with decoder
    decoder_.DecodePacket(data_, received);

    // pull next message
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        boost::bind(&Receiver::HandleReceiveFrom, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }
}
