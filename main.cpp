#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

void handle_client(asio::ssl::stream<tcp::socket>& ssl_socket) {
	ssl_socket.handshake(asio::ssl::stream_base::server);

	// Send a secure response
	std::string response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 13\r\n\r\n"
		"Hello, World!";

	asio::write(ssl_socket, asio::buffer(response));
}

int main() {
    try {
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 443));

	// Configure SSL context
	asio::ssl::context ssl_context(asio::ssl::context::tls_server);
	ssl_context.use_certificate_chain_file("server.crt");
	ssl_context.use_private_key_file("server.key", asio::ssl::context::pem);
	
        std::cout << "HTTPS Server is running on port 443..." << std::endl;

        while (true) {
		// Accept a connection
		tcp::socket socket(io_context);
		acceptor.accept(socket);

		// Wrap the socket with SSL
		asio::ssl::stream<tcp::socket> ssl_socket(std::move(socket), ssl_context);

		try {
			// Perform SSL handshake
			ssl_socket.handshake(asio::ssl::stream_base::server);

			// Handle the client request
			std::string response =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Content-Length: 13\r\n\r\n"
				"Hello, World!";
			asio::write(ssl_socket, asio::buffer(response));
		} catch (const std::exception& e) {
			std::cerr << "Error during client handling: " << e.what() << std::endl;
		}
        }
    } catch (const std::exception& e) {
        std::cerr << "Server Error: " << e.what() << std::endl;
        return 1;
    }
}
