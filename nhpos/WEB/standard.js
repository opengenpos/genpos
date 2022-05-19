//-----------------------------------------------------------------
// Simple Javascript to test the simple NeighborhoodPOS web server.
//   function createXHR ()
//   function getthefilebyid(szFileName, szElementId)
//   function postthephpbyid (szFileName, szElementId)
//-----------------------------------------------------------------

//-----------------------------------------------------------------------
// function createXHR ()
//
// Input:     NONE.
//
// Description:  Implement simple Ajax connection functionality
//               The function createXHR is used to create the communication
//               primitive that allows us to do either an HTTP GET or POST.
//               First of all, a Javascript function should call the createXHR
//               function to create the communication primitive and then use it
//               in a sequence to create an Ajax type of request.
function createXHR ()
{
	var xhr;
	try
	{
		xhr = new ActiveXObject("Msxml2.XMLHTTP");
	}
	catch (e)
	{
		try
		{
			xhr = new ActiveXObject("Microsoft.XMLHTTP");
		}
		catch (e)
		{
			xhr = false;
		}
	}
	if (!xhr && typeof XMLHttpRequest != 'undefined')
	{
		xhr = new XMLHttpRequest();
	}
	return xhr;
}

//-----------------------------------------------------------------------
// function getthefilebyid(szFileName, szElementId)
//
// Input:     szFileName -  string containing name of the file to GET
//            szElementId - string containing the document element id
//                          into which to put the downloaded file.
//
// Description:  This Javascript function will do a GET on a file.
//               If the file is a php script then the output of the php script
//               will be retrieved.  Otherwise the text in the particular file
//               will be retrieved.
function getthefilebyid(szFileName, szElementId)
{
	var xhr = createXHR ();
	xhr.open("GET", "/"+szFileName, false);
	xhr.send(null);
	
	if (xhr.readyState == 4)
	{
		var data = xhr.responseText;
		document.getElementById(szElementId).firstChild.nodeValue = data;
	}
	else
	{
		alert("xhr.readyState != 4");
	}
}

//-----------------------------------------------------------------------
// function postthephpbyid (szFileName, szElementId, szSendString)
//
// Input:     szFileName   - string containing name of the file to POST
//            szElementId  - string containing the document element id
//                           into which to put the downloaded file.
//            szSendString - string containing the text to send or POST
//
// Description:  This Javascript function performs a POST request and
//               should be used with a server side function that can interpret
//               the POST and send a response.
function postthephpbyid (szFileName, szElementId, szSendString)
{
	var xhr = createXHR ();
	xhr.open("POST", "/"+szFileName, false);
	xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	xhr.setRequestHeader("Connection", "close");

	xhr.send(szSendString);
	
	if (xhr.readyState == 4)
	{
		var data = xhr.responseText;
		document.getElementById(szElementId).firstChild.nodeValue = data;
	}
	else
	{
		alert("xhr.readyState != 4");
	}
}
