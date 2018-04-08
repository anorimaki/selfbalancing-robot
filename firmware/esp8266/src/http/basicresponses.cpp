#include "http/basicresponses.h"
#include "error.h"

namespace http
{

void sendError( AsyncWebServerRequest* request ) {
	String content = String("{\"error\":\"");
	while( !error::globalStack().empty() ) {
		const error::StackItem& item = error::globalStack().top();
		content.concat( item.file() );
		content.concat( "," );
		content.concat( item.line() );
		content.concat( ": " );
		content.concat( item.message().c_str() );
		error::globalStack().pop();
	}
	content.concat("\"}");

	request->send( 500, CONTENT_TYPE_JSON, content );
}

}
