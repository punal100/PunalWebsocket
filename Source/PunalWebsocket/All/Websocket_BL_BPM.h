// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Punal Manalan,
// NOTE:
// Include Errors Because Same Named Check Macro is Used in Unreal Engine As well
#pragma push_macro("check")
#undef check

// Punal Manalan,
// NOTE:
// Fixing Compile Error By Making This Execption Function

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
//#include <openssl/ssl.h>

//#pragma warning(disable: 4668)
//#include <boost/thread.hpp>

#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
namespace stdext
{
    class exception;
}
namespace boost
{
    void throw_exception(class stdext::exception const&, struct boost::source_location const&)
    {
        //throw std::exception{};
        BOOST_UNREACHABLE_RETURN(0);
    }

    void throw_exception(std::exception const& e)
    {
        //do nothing
        BOOST_UNREACHABLE_RETURN(0);
    }

    void throw_exception(std::exception const&, boost::source_location const&)
    {
        BOOST_UNREACHABLE_RETURN(0);
    }
}

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>

THIRD_PARTY_INCLUDES_END
#undef UI

// Punal Manalan,
// NOTE:
// Now Using Unreal Engine Version Of Check
#pragma pop_macro("check")

#include "Websocket_BL_BPM.generated.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

UCLASS(BlueprintType)
class UPunal_Log_Object : public UObject
{
    GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, Category = "Punal | Log")
        void Log_OnScreen(FString Log_Message)
    {
        AsyncTask(ENamedThreads::GameThread, [=]()
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Log_Message);
            });
    }
};

// Report a failure
void fail(boost::system::error_code ec, char const* what, UPunal_Log_Object* Arg_Log_Obj)
{
    FString Temp_String = "Punal_Log: ";
    Temp_String += what;
    Temp_String += ": ";
    Temp_String += FString(ec.message().c_str());

    Arg_Log_Obj->Log_OnScreen(Temp_String);
}

// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string text_;

public:
    UPunal_Log_Object* Log_Obj = nullptr;
    // Resolver and socket require an io_context
    explicit
        session(net::io_context& ioc)
        : resolver_(ioc.get_executor())
        , ws_(ioc.get_executor())
    {
        //Log_Obj = Arg_Log_Obj;
    }

    // Start the asynchronous operation
    void
        run(
            char const* host,
            char const* port,
            char const* text)
    {
        // Save these for later
        host_ = host;
        text_ = text;

        // Look up the domain name
        resolver_.async_resolve(
            host,
            port,
            beast::bind_front_handler(
                &session::on_resolve,
                shared_from_this()));
    }

    void
        on_resolve(
            beast::error_code ec,
            tcp::resolver::results_type results)
    {
        if (ec)
            return fail(ec, "resolve", Log_Obj);

        // Set the timeout for the operation
        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(ws_).async_connect(
            results,
            beast::bind_front_handler(
                &session::on_connect,
                shared_from_this()));
    }

    void
        on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
    {
        if (ec)
            return fail(ec, "connect", Log_Obj);

        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(ws_).expires_never();

        // Set suggested timeout settings for the websocket
        ws_.set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::client));

        // Set a decorator to change the User-Agent of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-async");
            }));

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host_ += ':' + std::to_string(ep.port());

        // Perform the websocket handshake
        ws_.async_handshake(host_, "/",
            beast::bind_front_handler(
                &session::on_handshake,
                shared_from_this()));
    }

    void
        on_handshake(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "handshake", Log_Obj);

        // Send the message
        ws_.async_write(
            net::buffer(text_),
            beast::bind_front_handler(
                &session::on_write,
                shared_from_this()));
    }

    void
        on_write(
            beast::error_code ec,
            std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write", Log_Obj);

        // Read a message into our buffer
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &session::on_read,
                shared_from_this()));
    }

    void
        on_read(
            beast::error_code ec,
            std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read", Log_Obj);

        // Close the WebSocket connection
        ws_.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &session::on_close,
                shared_from_this()));
    }

    void
        on_close(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "close", Log_Obj);

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence

        FString Temp_String = "Punal_Log: ";
        Temp_String += FString(
            boost::beast::buffers_to_string
            (
                buffer_.data()
            ).c_str()
        );

        Log_Obj->Log_OnScreen(Temp_String);
    }
};

USTRUCT(BlueprintType)
struct FPunal_Websocket
{
    GENERATED_BODY()
public:
    std::shared_ptr<session> Websocket_Ptr = nullptr;
    //std::shared_ptr<boost::asio::io_context> ioc = nullptr;
};

/**
 * 
 */
UCLASS()
class PUNALWEBSOCKET_API UWebsocket_BL_BPM : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "Punal | Blueprint Library | Websocket")
        static FString Get_Platform_SSL_Public_Certificates();

    UFUNCTION(BlueprintCallable, Category = "Punal | Blueprint Library | Websocket")
        static FPunal_Websocket Create_And_Start_Websocket_Blocking(FString IP, int Port, FString Text);

    UFUNCTION(BlueprintCallable, Category = "Punal | Blueprint Library | Websocket")
        static FPunal_Websocket Create_And_Start_Websocket_Non_Blocking(FString IP, int Port, FString Text, UPunal_Log_Object* Log_Obj);

    UFUNCTION(BlueprintCallable, Category = "Punal | Blueprint Library | Websocket")
        static FPunal_Websocket Create_And_Start_Websocket_SSL_Blocking(FString Host, int Port, FString Text);

    //UFUNCTION(BlueprintCallable, Category = "Punal | Blueprint Library | Websocket")
        static void Close_Target_Websocket(FPunal_Websocket Arg_Websocket);
};
