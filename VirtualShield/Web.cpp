
#include "Sensor.h"
#include "Web.h"
#include "SensorModels.h"

extern "C" {
#include <string.h>
#include <stdlib.h>
}

const PROGMEM char SERVICE_WEB[] = "WEB";
const PROGMEM char GET[] = "Get";
const PROGMEM char POST[] = "Post";
const PROGMEM char DATA[] = "Data";

const PROGMEM char LEN[] = "Len";

/// <summary>
/// Initializes a new instance of the <see cref="Web"/> class.
/// </summary>
/// <param name="shield">The shield.</param>
Web::Web(const VirtualShield &shield) : Sensor(shield, 'W') {
}

/// <summary>
/// Performs a web Get, optionally returning a result.
/// </summary>
/// <param name="url">The url.</param>
/// <param name="parsingInstructions">The parsing instructions.</param>
/// <param name="maxLength">The maximum length of the result.</param>
/// <returns>The id of the message. Negative if an error.</returns>
int Web::get(String url, String parsingInstructions, int maxLength)
{
	return get(EPtr(MemPtr, URL, url.c_str()), EPtr(parsingInstructions ? MemPtr : None, PARSE, parsingInstructions.c_str()), maxLength);
}

/// <summary>
/// Performs a web Post, optionally returning a result.
/// </summary>
/// <param name="url">The url.</param>
/// <param name="data">The data.</param>
/// <param name="parsingInstructions">The parsing instructions.</param>
/// <param name="maxLength">The maximum length of the result.</param>
/// <returns>The id of the message. Negative if an error.</returns>
int Web::post(String url, String data, String parsingInstructions, int maxLength)
{
	return post(EPtr(MemPtr, URL, url.c_str()), EPtr(MemPtr, DATA, data.c_str()), EPtr(parsingInstructions ? MemPtr : None, PARSE, parsingInstructions.c_str()), maxLength);
}

/// <summary>
/// Performs a web Get, optionally returning a result.
/// </summary>
/// <param name="url">The url.</param>
/// <param name="parsingInstructions">The parsing instructions.</param>
/// <param name="maxLength">The maximum length of the result.</param>
/// <returns>The id of the message. Negative if an error.</returns>
int Web::get(EPtr url, EPtr parsingInstructions, int maxLength)
{
	EPtr eptrs[] = { EPtr(ACTION, GET), url,
		EPtr(LEN, maxLength),
		parsingInstructions };
	return shield.block(writeAll(SERVICE_WEB, eptrs, 4), onEvent == 0);
}

/// <summary>
/// Performs a web Post, optionally returning a result.
/// </summary>
/// <param name="url">The url.</param>
/// <param name="data">The data.</param>
/// <param name="parsingInstructions">The parsing instructions.</param>
/// <param name="maxLength">The maximum length of the result.</param>
/// <returns>The id of the message. Negative if an error.</returns>
int Web::post(EPtr url, EPtr data, EPtr parsingInstructions, int maxLength)
{
	EPtr eptrs[] = { EPtr(ACTION, POST), url, data,
		EPtr(LEN, maxLength),
		parsingInstructions };
	return shield.block(writeAll(SERVICE_WEB, eptrs, 5), onEvent == 0);
}

/// <summary>
/// Copies the current web response (only valid before another web event) into a supplied buffer.
/// </summary>
/// <param name="responseBuffer">The buffer to place the response.</param>
/// <param name="length">The maximum length of the buffer.</param>
/// <param name="parts">Array of character pointers to populate after parsing.</param>
/// <param name="partCount">The part count.</param>
void Web::getResponse(char* responseBuffer, int length, char** parts, int partCount)
{
	strncpy(responseBuffer, response, length);
	if (parts && partCount > 0)
	{
		int count = 0;

		int index = 0;
		parts[count++] = &responseBuffer[index];

		while (count < partCount && ++index<length && responseBuffer[index])
		{
			if (responseBuffer[index] == '|')
			{
				parts[count++] = &responseBuffer[index + 1];
				responseBuffer[index] = 0;
			}
		}
	}
}

/// <summary>
/// Event called when a valid json message was received. 
/// Consumes the proper values for this sensor.
/// </summary>
/// <param name="root">The root json object.</param>
/// <param name="shieldEvent">The shield event.</param>
void Web::onJsonReceived(JsonObject& root, ShieldEvent* shieldEvent)
{
	response = shieldEvent->result;
	resultId = shieldEvent->resultId;
	Sensor::onJsonReceived(root, shieldEvent);
}