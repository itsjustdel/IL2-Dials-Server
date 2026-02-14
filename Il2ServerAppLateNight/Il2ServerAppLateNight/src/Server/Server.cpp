#pragma warning(disable:4996)

#define _WIN32_WINNT 0x0601
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <sstream>

#include "../Main/Main.h"
#include "../IPHelper/IPHelper.h"
#include "../OilTemp/OilTemp.h"
#include "../CylinderTemp//CylinderHead.h"
#include "../CarbMixTemp/CarbMixTemp.h"

using namespace System::Diagnostics;
using boost::asio::ip::udp;

int nServerPort = 11200;

void SetPortNumber(int t)
{
	nServerPort = t;
}

int serverBroadcast()
{
	try {
		boost::asio::io_context io_context;
		udp::socket sock(io_context, udp::endpoint(udp::v4(), 0));
		
		sock.set_option(boost::asio::socket_base::broadcast(true));

		udp::endpoint broadcast_endpoint(boost::asio::ip::address_v4::broadcast(), nServerPort);
		
		char sendMSG[] = "Broadcast message from IL-2 Dials Server";

		while (true)
		{
			sock.send_to(boost::asio::buffer(sendMSG, strlen(sendMSG) + 1), broadcast_endpoint);
		}

		sock.close();
	}
	catch (std::exception& e) {
		std::cerr << "Exception in serverBroadcast: " << e.what() << std::endl;
		return 0;
	}
	
	return 1;
}

int serverListen() {
	try {
		// Create io_context for Boost.Asio operations
		boost::asio::io_context io_context;
		
		// Create UDP socket and bind to any address on the specified port
		udp::socket socket(io_context, udp::endpoint(udp::v4(), nServerPort));
		
		// Buffer for receiving data
		char RecvBuf[1024];
		udp::endpoint sender_endpoint;

		wprintf(L"Server listening on port %d...\n", nServerPort);

		while (true)
		{
			// Receive datagram from client
			boost::system::error_code error;
			size_t len = socket.receive_from(
				boost::asio::buffer(RecvBuf, 1024), 
				sender_endpoint, 
				0, 
				error
			);

			if (error && error != boost::asio::error::message_size)
			{
				OutputDebugString(L"receive_from failed\n");
				continue;
			}

			wprintf(L"Receiving datagrams...\n");

			////////////////Package//////////////////////
			//we represent the data with floats in the app, so let's convert now and save network traffic
			const int total = 38;
			float floatArray[total];

			//read memory only when requested - could be refactored in to the getters
			//HANDLE hProcess = GetIL2Handle();
			//LPVOID codeCaveAddress = GetCodeCaveAddress();
			ReadPlaneType();
			ReadCockpitInstruments();
			ReadAltimeter();

			ReadTurnCoordinatorBall();
			ReadManifolds();
			ReadEngineModification();
			//water temps read in water temps class - TO DO, refactor for above
			UpdateTurnNeedle();
			UpdateWaterTempValues();
			UpdateOilTempValues();
			UpdateCylinderHeadTemps();
			UpdateCarbMixTemps();


			//packing different data types in to one char array for sending (serialisation)
			//https://stackoverflow.com/questions/1703322/serialize-strings-ints-and-floats-to-character-arrays-for-networking-without-li

			//version
			float programVersion = GetIL2DialsVersion();
			//planeType
			std::string planeType = GetPlaneType();

			//send blanks if no game found/injected
			if (GetInjected() == false)
			{
				planeType = "No Plane";

				for (SIZE_T i = 0; i < total; i++)
				{
					floatArray[i] = 0;
				}
				floatArray[1] = 760;//mmhg default
			}
			else
			{
				//if we have found the altimeter struct we can read from here, this allows us to get the needle position as it moves so we don't need to calculate that ourselves
				floatArray[0] = (float)(GetAltitude());
				//mgh
				floatArray[1] = (float)(GetMMHg());
				//airspeed
				floatArray[2] = (float)(GetAirspeedFromCockpitStruct());
				//heading
				floatArray[3] = (float)(GetHeading());
				//Pitch
				floatArray[4] = (float)(GetPitch());
				//Roll
				floatArray[5] = (float)(GetRoll());
				//vertical speed 
				floatArray[6] = (float)(GetVSI());
				//ball
				floatArray[7] = (float)(GetTurnAndBankBall());
				//bank needle
				floatArray[8] = (float)(GetTurnAndBankNeedle());
				//rpm
				for (SIZE_T i = 0; i < 4; i++)//4 engines max
				{
					//get rpm know where the rpm struct starts
					floatArray[9 + i] = (float)(GetRPM(i));
				}
				//manifold(s)
				for (SIZE_T i = 0; i < 4; i++)//4 engines max
				{
					floatArray[13 + i] = (float)(GetManifold(i));
				}
				//engine mod
				floatArray[17] = (float)(GetEngineModification());

				//water temp
				for (SIZE_T i = 0; i < 4; i++)
				{
					floatArray[18 + i] = (float)(GetWaterTemp(i));
				}
				//oil temp out
				for (SIZE_T i = 0; i < 4; i++)
				{
					floatArray[22 + i] = (float)(GetOilTempOut(i));
				}
				//oil temp In
				for (SIZE_T i = 0; i < 4; i++)
				{
					floatArray[26 + i] = (float)(GetOilTempIn(i));
				}
				//cylinder head temp
				for (SIZE_T i = 0; i < 4; i++)
				{
					floatArray[30 + i] = (float)(GetCylinderHeadTemp(i));
				}
				//carb mix temp
				for (SIZE_T i = 0; i < 4; i++)
				{
					floatArray[34 + i] = (float)(GetCarbMixTemp(i));
				}
			}

			// The buffer we will be writing bytes into
			//make space for
			//program version
			//planetype string size
			//planetype string data size
			//float array containing instrument/dial values
			//64 bit string
			unsigned char outBuf[sizeof(uint32_t) + sizeof(uint32_t) + 64 + sizeof(floatArray)];
			// A pointer we will advance whenever we write data
			unsigned char* p = outBuf;

			//float array to buffer - NOTE THESE MUST STAY THE FIRST POSITON IN THE STREAM - 3RD PARTY DEPENDENCIES
			//we know how big this float array will be so we don't need to send size
			memcpy(p, (char*)floatArray, sizeof(floatArray));
			p += sizeof(floatArray);//4btye float * array length

			// Serialize "program version" into outBuf
			//using uint32_t when serializing            
			//copy float to uint32 safely
			uint32_t fbits = 0;
			memcpy(&fbits, &programVersion, sizeof(programVersion));
			//copy to buffer
			memcpy(p, &fbits, sizeof(fbits));
			p += sizeof(fbits);

			// Serialize "planeType string length" into outBuf
			//SIZE_T to uint32
			uint32_t neX = static_cast<unsigned int>(planeType.size());//note it is SIZE_T and not the size of the data in the array
			memcpy(p, &neX, sizeof(neX));
			p += sizeof(neX);

			//string data already in correct format - copy to buffer            
			memcpy(p, planeType.data(), 64);
			p += 64;

			char* sendBuffer = (char*)outBuf;
			int sendLength = sizeof(outBuf);

			// send to client (sender is the client, it initiated the request)         
			boost::system::error_code send_error;
			socket.send_to(
				boost::asio::buffer(sendBuffer, sendLength), 
				sender_endpoint, 
				0, 
				send_error
			);

			if (send_error) {
				OutputDebugString(L"send_to failed\n");
			}
		}

		//-----------------------------------------------
		// Close the socket when finished receiving datagrams
		OutputDebugString(L"Finished receiving. Closing socket.\n");
		socket.close();
	}
	catch (std::exception& e) {
		std::cerr << "Exception in serverListen: " << e.what() << std::endl;
		OutputDebugString(L"serverListen failed with exception\n");
		return 1;
	}

	//-----------------------------------------------
	// Clean up and exit.
	OutputDebugString(L"Exiting.\n");
	return 0;

}

boost::asio::ip::address CreateServerAddress(bool localIP)
{
	// Get local IP address
	// Note: This helper function is currently not used by serverListen(),
	// which binds to all interfaces (INADDR_ANY) to accept connections from any network adapter.
	std::vector<std::string> ipv4Addreses = GetIPAddresses(localIP);
	if (ipv4Addreses.size() == 0)
	{
		// Return "any address" (0.0.0.0) as fallback - binds to all interfaces
		OutputDebugString(L"Warning: No IP addresses found, using any address (0.0.0.0)\n");
		return boost::asio::ip::address_v4::any();
	}

	std::string ipAddress = ipv4Addreses[0];
	
	// Convert string to Boost.Asio address
	boost::system::error_code ec;
	auto addr = boost::asio::ip::address::from_string(ipAddress, ec);
	
	if (ec) {
		// Return "any address" on parse error
		OutputDebugString(L"Warning: Failed to parse IP address, using any address (0.0.0.0)\n");
		return boost::asio::ip::address_v4::any();
	}
	
	return addr;
}

int StartServerUDP(System::ComponentModel::BackgroundWorker^ worker, bool localIP)
{
	// Note: 'worker' and 'localIP' parameters are kept for backward compatibility with the UI layer
	// but are currently unused. The server binds to all network interfaces (INADDR_ANY).
	// The worker parameter was likely intended for progress reporting, and localIP for binding
	// to a specific interface, but these features are not currently implemented.
	(void)worker;  // Suppress unused parameter warning
	(void)localIP; // Suppress unused parameter warning
	
	serverListen();

	return 1;
}