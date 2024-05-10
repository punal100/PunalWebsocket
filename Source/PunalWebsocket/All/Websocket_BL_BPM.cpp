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

FPunal_Websocket UWebsocket_BL_BPM::Create_And_Start_Websocket(FString IP, int Port, FString Text)
{
    FPunal_Websocket Return_Websocket;
    FString Port_String = FString::FromInt(Port);

    auto const host = TCHAR_TO_ANSI(*IP);
    auto const port = TCHAR_TO_ANSI(*Port_String);
    auto const text = TCHAR_TO_ANSI(*Text);

    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    //std::shared_ptr<boost::asio::io_context> ptr_ioc = std::make_shared<boost::asio::io_context>();
    //Return_Websocket.ioc = ptr_ioc;
    //Return_Websocket.ioc = std::make_shared<boost::asio::io_context>();

    // Launch the asynchronous operation
    //std::shared_ptr<session> ptr = std::make_shared<session>(Return_Websocket.ioc);
    //Return_Websocket.Websocket_Ptr = ptr;
   
    Return_Websocket.Websocket_Ptr = std::make_shared<session>(ioc);
    Return_Websocket.Websocket_Ptr.get()->run(host, port, text);

    // Run the I/O service. The call will return when
    // the get operation is complete.
    ioc.run();

    return Return_Websocket;
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
    Arg_Websocket.Websocket_Ptr.get()->close();
}