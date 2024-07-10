// Fill out your copyright notice in the Description page of Project Settings.


#include "Websocket_BL_BPM.h"

#include "Misc/ConfigCacheIni.h"

#if PLATFORM_WINDOWS
THIRD_PARTY_INCLUDES_START
//#define NOMINMAX
#include <Windows.h>
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <wincrypt.h>
THIRD_PARTY_INCLUDES_END
#elif PLATFORM_ANDROID
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
//#elif
#endif

FString UWebsocket_BL_BPM::Get_Platform_SSL_Public_Certificates()
{
    bool bUsePlatformProvidedCertificates = true;
    if (GConfig->GetBool(TEXT("SSL"), TEXT("bUsePlatformProvidedCertificates"), bUsePlatformProvidedCertificates, GEngineIni))
    {
        if (!bUsePlatformProvidedCertificates)
        {
            return "";
        }
    }

    FString All_Cert = "";

#if PLATFORM_WINDOWS
    // Open Windos Cert Store
    HCERTSTORE SystemRootStore = CertOpenSystemStoreW(0, L"ROOT");
    if (SystemRootStore)
    {
        std::wstring Pem = L"0";
        std::string convertedString = "";
        DWORD num = 1;

        PCCERT_CONTEXT pCert = nullptr;
        while (true)
        {
            pCert = CertEnumCertificatesInStore(SystemRootStore, pCert);
            if (!pCert)
            {
                break;
            }

            DWORD size = 0;
            CryptBinaryToString(pCert->pbCertEncoded, pCert->cbCertEncoded, CRYPT_STRING_BASE64HEADER, nullptr, &size);
            
            Pem.resize(size);
            CryptBinaryToString(pCert->pbCertEncoded, pCert->cbCertEncoded, CRYPT_STRING_BASE64HEADER,
                Pem.data(), &size);

            convertedString = (std::string(Pem.begin(), Pem.end()));
            All_Cert.Append(convertedString.c_str());

            num = num + 1;
        }

        CertCloseStore(SystemRootStore, 0);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Unable to open ROOT certificate store. Platform provided certificates will not be used"));
        return "";
    }
#elif PLATFORM_ANDROID
    // gather all the files in system certificates directory
    TArray<FString> DirectoriesToIgnoreAndNotRecurse;
    FLocalTimestampDirectoryVisitor Visitor(FPlatformFileManager::Get().GetPlatformFile(), DirectoriesToIgnoreAndNotRecurse, DirectoriesToIgnoreAndNotRecurse, false);
    IFileManager::Get().IterateDirectory(TEXT("/system/etc/security/cacerts"), Visitor);

    for (const TPair<FString, FDateTime>& FileTimePair : Visitor.FileTimes)
    {
        FString FileName = FileTimePair.Key;
        UE_LOG(LogTemp, Log, TEXT("Checking if '%s' exists"), *FileName);

        if (FPaths::FileExists(FileName))
        {
            UE_LOG(LogTemp, Log, TEXT("Loading certificates from %s"), *FileName);

            int64 CertificateBundleBufferSize = 0;
            TUniquePtr<char[]> CertificateBundleBuffer;

            if (TUniquePtr<FArchive> CertificateBundleArchive = TUniquePtr<FArchive>(IFileManager::Get().CreateFileReader(*FileName, 0)))
            {
                CertificateBundleBufferSize = CertificateBundleArchive->TotalSize();
                CertificateBundleBuffer.Reset(new char[CertificateBundleBufferSize + 1]);
                CertificateBundleArchive->Serialize(CertificateBundleBuffer.Get(), CertificateBundleBufferSize);
                CertificateBundleBuffer[CertificateBundleBufferSize] = '\0';
            }

            if (CertificateBundleBufferSize > 0 && CertificateBundleBuffer != nullptr)
            {
                All_Cert.Append(FString(CertificateBundleBuffer.Get(), CertificateBundleBufferSize));
            }
        }
    }
#else
    static const TCHAR* KnownBundlePaths[] =
    {
        TEXT("/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem"),    // CentOS/RHEL 7
        TEXT("/etc/ssl/certs/ca-certificates.crt"),                   // Debian/Ubuntu/Gentoo etc.
        TEXT("/etc/pki/tls/certs/ca-bundle.crt"),                     // Fedora/RHEL 6
        TEXT("/system/etc/security/cacerts"),                         // Android
        TEXT("/etc/pki/tls/cacert.pem"),                              // OpenELEC
        TEXT("/etc/ssl/ca-bundle.pem"),                               // OpenSUSE
        TEXT("/usr/local/share/certs"),                               // FreeBSD
        TEXT("/etc/pki/tls/certs"),                                   // Fedora/RHEL
        TEXT("/etc/openssl/certs"),                                   // NetBSD
        TEXT("/etc/ssl/cert.pem"),                                    // Alpine Linux
        TEXT("/etc/ssl/certs"),                                       // SLES10/SLES11, https://golang.org/issue/12139
        TEXT("/var/ssl/certs"),                                       // AIX
    };
    All_Cert = "";
    for (const TCHAR* CurrentBundle : KnownBundlePaths)
    {
        FString FileName(CurrentBundle);
        UE_LOG(LogTemp, Log, TEXT("Checking if '%s' exists"), *FileName);

        if (FPaths::FileExists(FileName))
        {
            UE_LOG(LogTemp, Log, TEXT("Loading certificates from %s"), *FileName);

            int64 CertificateBundleBufferSize = 0;
            TUniquePtr<char[]> CertificateBundleBuffer;

            if (TUniquePtr<FArchive> CertificateBundleArchive = TUniquePtr<FArchive>(IFileManager::Get().CreateFileReader(*FileName, 0)))
            {
                CertificateBundleBufferSize = CertificateBundleArchive->TotalSize();
                CertificateBundleBuffer.Reset(new char[CertificateBundleBufferSize + 1]);
                CertificateBundleArchive->Serialize(CertificateBundleBuffer.Get(), CertificateBundleBufferSize);
                CertificateBundleBuffer[CertificateBundleBufferSize] = '\0';
            }

            if (CertificateBundleBufferSize > 0 && CertificateBundleBuffer != nullptr)
            {
                All_Cert.Append(FString(CertificateBundleBuffer.Get(), CertificateBundleBufferSize));
            }
        }
    }
#endif
    return All_Cert;
}

FPunal_Websocket UWebsocket_BL_BPM::Create_And_Start_Websocket_Blocking(FString IP, int Port, FString Text)
{
    FPunal_Websocket Return_Websocket;
    
    FString Port_String = FString::FromInt(Port);

    FString host = IP;
    FString port = Port_String;
    FString text = Text;

    // The io_context is required for all I/O
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver{ ioc };
    websocket::stream<tcp::socket> ws{ ioc };

    // Look up the domain name
    auto const results = resolver.resolve(TCHAR_TO_UTF8(*host), TCHAR_TO_UTF8(*port));

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
    ws.write(net::buffer(std::string(TCHAR_TO_UTF8(*text))));

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
    
    return Return_Websocket;
}

FPunal_Websocket UWebsocket_BL_BPM::Create_And_Start_Websocket_Non_Blocking(FString IP, int Port, FString Text, UPunal_Log_Object* Log_Obj)
{
    // Create new Thread
    //boost::shared_ptr<boost::thread> m_thread;
    
    //m_thread = boost::make_shared<boost::thread>([=]()
    AsyncTask(ENamedThreads::AnyThread, [=]()
        {
            FString Port_String = FString::FromInt(Port);

            FString host = IP;
            FString port = Port_String;
            FString text = Text;

            // The io_context is required for all I/O
            net::io_context ioc;

            //Create a Shared_Ptr
            //session* f = new session(ioc);
            boost::shared_ptr<session> Websocket_Session_Ptr = boost::make_shared<session>(ioc);
            Websocket_Session_Ptr->Log_Obj = Log_Obj;

            // Launch the asynchronous operation
            std::make_shared<session>(ioc)->run(TCHAR_TO_UTF8(*host), TCHAR_TO_UTF8(*port), TCHAR_TO_UTF8(*text));
            //m_thread = boost::make_shared<boost::thread>(boost::bind(&session::run, Websocket_Session_Ptr, host, port, text));
            //m_thread = boost::make_shared<boost::thread>(boost::bind(&session::test, Websocket_Session_Ptr));
            //m_thread = new boost::thread(boost::bind(&session::test, Websocket_Session_Ptr));
            //m_thread = new boost::thread(boost::bind(&session::test, &Websocket_Session_Ptr));

            //m_thread->join();

            // Run the I/O service. The call will return when
            // the socket is closed.
            ioc.run();

            //return FPunal_Websocket();            
        }
    );

    return FPunal_Websocket();
}

FPunal_Websocket UWebsocket_BL_BPM::Create_And_Start_Websocket_SSL_Blocking(FString Host, int Port, FString Text)
{
    FString Temp_String = "";

    FPunal_Websocket Return_Websocket;

    FString Port_String = FString::FromInt(Port);

    FString host = Host;
    std::string host_std = TCHAR_TO_UTF8(*host);
    FString port = Port_String;
    FString text = Text;

    // The io_context is required for all I/O
    net::io_context ioc;

    // The SSL context is required, and holds certificates
    ssl::context ctx{ ssl::context::tlsv12_client };

    //// This holds the root certificate used for verification
    //load_root_certificates(ctx);
    //FSslModule::Get().GetCertificateManager().VerifySslCertificates(Context, Domain);// Found inside Unreal Engine Websocket
    {
        boost::system::error_code ec;
        FString cert = "";
        cert = Get_Platform_SSL_Public_Certificates();

        std::string cert_stdstring = TCHAR_TO_UTF8(*cert);

        ctx.add_certificate_authority(
            boost::asio::buffer(cert_stdstring.data(), cert_stdstring.size()), ec);
        if (ec)
        {
            Temp_String = "Punal_Log: Cerfiticate Add Failed: ";
            Temp_String += FString(ec.message().c_str());
            //Temp_String += " Input Cert: ";
            //Temp_String += cert;

            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);
            return Return_Websocket;
        }
    }

    // These objects perform our I/O
    tcp::resolver resolver{ ioc };
    websocket::stream<beast::ssl_stream<tcp::socket>> ws{ ioc, ctx };

    // Look up the domain name
    auto const results = resolver.resolve(host_std, TCHAR_TO_UTF8(*port));

    // Make the connection on the IP address we get from a lookup
    boost::asio::ip::tcp::endpoint ep = net::connect(beast::get_lowest_layer(ws), results);

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host_std.c_str()))
    {
        Temp_String = "Punal_Log: ";
        Temp_String += beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()).message().c_str();
        Temp_String += " Failed to set SNI Hostname";
    }

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host += ":" + FString::FromInt(ep.port());

    // Perform the SSL handshake
    ws.next_layer().handshake(ssl::stream_base::client);

    // Set a decorator to change the User-Agent of the handshake
    ws.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set(http::field::user_agent,
            std::string(BOOST_BEAST_VERSION_STRING) +
            " websocket-client-cor");
        }));

    // Perform the websocket handshake
    ws.handshake(host_std, "/");

    // Send the message
    ws.write(net::buffer(std::string(TCHAR_TO_UTF8(*text))));

    // This buffer will hold the incoming message
    beast::flat_buffer buffer;

    // Read a message into our buffer
    ws.read(buffer);

    // Read Twice, JUST FOR TESTING
    ws.read(buffer);

    // Close the WebSocket connection
    ws.close(websocket::close_code::normal);

    // If we get here then the connection is closed gracefully

    // The make_printable() function helps print a ConstBufferSequence
    Temp_String = "Punal_Log: ";
    Temp_String += FString(
        boost::beast::buffers_to_string
        (
            buffer.data()
        ).c_str()
    );
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Temp_String);

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
    //Arg_Websocket.Websocket_Ptr.get()->close();
}