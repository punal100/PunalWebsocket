// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Punal Manalan,
// NOTE:
// Include Errors Because Same Named Check Macro is Used in Unreal Engine As well
#pragma push_macro("check")
#undef check

#include <Boost/boost-1_80_0/include/boost/beast/websocket.hpp>
#include <Boost/boost-1_80_0/include/boost/beast/core.hpp>
#include <Boost/boost-1_80_0/include/boost/beast/websocket.hpp>
#include <Boost/boost-1_80_0/include/boost/asio/connect.hpp>
#include <Boost/boost-1_80_0/include/boost/asio/ip/tcp.hpp>

// Punal Manalan,
// NOTE:
// Now Using Unreal Engine Version Of Check
#pragma pop_macro("check")

#include "Websocket_BL_BPM.generated.h"

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>








// Report a failure
void fail(boost::system::error_code ec, char const* what)
{
    FString Temp_String = "Punal_Log: ";
    Temp_String += what;
    Temp_String += ": ";
    Temp_String += FString(ec.message().c_str());

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);
}

// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    websocket::stream<tcp::socket> ws_;
    boost::beast::multi_buffer buffer_;
    std::string host_;
    std::string text_;

public:
    // Resolver and socket require an io_context
    explicit
        session(boost::asio::io_context& ioc)
        : resolver_(ioc)
        , ws_(ioc)
    {
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
        close();
        close();
        close();
        close();
        close();
        // Look up the domain name
        resolver_.async_resolve(
            host,
            port,
            std::bind(
                &session::on_resolve,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
        on_resolve(
            boost::system::error_code ec,
            tcp::resolver::results_type results)
    {
        if (ec)
            return fail(ec, "resolve");

        // Make the connection on the IP address we get from a lookup
        boost::asio::async_connect(
            ws_.next_layer(),
            results.begin(),
            results.end(),
            std::bind(
                &session::on_connect,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
        on_connect(boost::system::error_code ec)
    {
        if (ec)
            return fail(ec, "connect");

        // Perform the websocket handshake
        ws_.async_handshake(host_, "/",
            std::bind(
                &session::on_handshake,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
        on_handshake(boost::system::error_code ec)
    {
        if (ec)

            close();
        close();
        close();
        close();
        close();
        close();
            return fail(ec, "handshake");

        // Send the message
        ws_.async_write(
            boost::asio::buffer(text_),
            std::bind(
                &session::on_write,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
        on_write(
            boost::system::error_code ec,
            std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        // Read a message into our buffer
        ws_.async_read(
            buffer_,
            std::bind(
                &session::on_read,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
        on_read(
            boost::system::error_code ec,
            std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        // Close the WebSocket connection
        ws_.async_close(websocket::close_code::normal,
            std::bind(
                &session::on_close,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
        on_close(boost::system::error_code ec)
    {
        if (ec)
            return fail(ec, "close");

        // If we get here then the connection is closed gracefully

        FString Temp_String = "Punal_Log: ";
        // The buffers() function helps print a ConstBufferSequence

        Temp_String += FString(
            boost::beast::buffers_to_string
            (
                buffer_.data()
            ).c_str()
        );

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);
    }

    void close()
    {
        // Close the WebSocket connection
        ws_.async_close(websocket::close_code::normal,
            std::bind(
                &session::on_close,
                shared_from_this(),
                std::placeholders::_1));
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
	    static FPunal_Websocket Create_And_Start_Websocket(FString IP, int Port, FString Text);

    UFUNCTION(BlueprintCallable, Category = "Punal | Blueprint Library | Websocket")
        static void Close_Target_Websocket(FPunal_Websocket Arg_Websocket);
};
