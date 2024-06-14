// Fill out your copyright notice in the Description page of Project Settings.


#include "Websocket_BL_BPM.h"

// Punal Manalan,
// NOTE:
// Fixing Compile Error By Making This Execption Function

namespace stdext
{
	class exception;
}
namespace boost
{
	void throw_exception(class stdext::exception const&, struct boost::source_location const&)
	{
		//throw std::exception{};
	}

	void throw_exception(std::exception const&, boost::source_location const&)
	{
		BOOST_UNREACHABLE_RETURN(0);
	}
}

FPunal_Websocket UWebsocket_BL_BPM::Create_And_Start_Websocket_Blocking(FString IP, int Port, FString Text)
{
    FPunal_Websocket Return_Websocket;
    try
    {
        FString Port_String = FString::FromInt(Port);

        FString host = TCHAR_TO_ANSI(*IP);
        auto const port = TCHAR_TO_ANSI(*Port_String);
        auto const text = TCHAR_TO_ANSI(*Text);

        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ ioc };
        websocket::stream<tcp::socket> ws{ ioc };

        // Look up the domain name
        auto const results = resolver.resolve(TCHAR_TO_UTF8(*host), port);

        // Make the connection on the IP address we get from a lookup
        auto ep = net::connect(ws.next_layer(), results);

        // Update the host_ string. This will provide the value of the
        // Host HTTP header during the WebSocket handshake.
        // See https://tools.ietf.org/html/rfc7230#section-5.4
        host += ":" + FString::FromInt(ep.port());

        // Set a decorator to change the User-Agent of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " websocket-client-coro");
            }));

        // Perform the websocket handshake
        ws.handshake(TCHAR_TO_UTF8(*host), "/");

        // Send the message
        ws.write(net::buffer(std::string(text)));

        // This buffer will hold the incoming message
        beast::flat_buffer buffer;

        // Read a message into our buffer
        ws.read(buffer);

        // Close the WebSocket connection
        ws.close(websocket::close_code::normal);

        // If we get here then the connection is closed gracefully

        // The make_printable() function helps print a ConstBufferSequence

        FString Temp_String = "Punal_Log: ";
        Temp_String += FString(
            boost::beast::buffers_to_string
            (
                buffer.data()
            ).c_str()
        );
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);
    }
    catch (std::exception const& e)
    {
        FString Temp_String = "Punal_Log: ";
        Temp_String += e.what();

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);

        return Return_Websocket;
    }
    return Return_Websocket;
}

FPunal_Websocket UWebsocket_BL_BPM::Create_And_Start_Websocket_Non_Blocking(FString IP, int Port, FString Text)
{
    FString Port_String = FString::FromInt(Port);

    auto const host = TCHAR_TO_ANSI(*IP);
    auto const port = TCHAR_TO_ANSI(*Port_String);
    auto const text = TCHAR_TO_ANSI(*Text);

    // The io_context is required for all I/O
    net::io_context ioc;

    // Launch the asynchronous operation
    std::make_shared<session>(ioc)->run(host, port, text);

    // Run the I/O service. The call will return when
    // the socket is closed.
    ioc.run();

    return FPunal_Websocket();
}

void UWebsocket_BL_BPM::Close_Target_Websocket(FPunal_Websocket Arg_Websocket)
{
    FString Temp_String = "";
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);

    if (Arg_Websocket.Websocket_Ptr.get() == nullptr)
    {
        Temp_String = "Punal_Log: Closing Websocket Externally Failed, Websocket is Nullptr";
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);

        return;
    }

    Temp_String = "Punal_Log: Attemptring to CLose Websocket";
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);
    //Arg_Websocket.Websocket_Ptr.get()->close();
}