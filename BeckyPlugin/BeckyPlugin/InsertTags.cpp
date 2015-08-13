////////////////////////////////////////////////////////////////////////////////////
// Template file for plugin.
//
// You can modify and redistribute this file without any permission.
//
// Note:
// Create a sub folder under "PlugInSDK" folder. e.g. "PlugInSDK\MyProject\" and
// place your project files there.

#include <windows.h>
#include "../BeckyAPI.h"
#include "../BkCommon.h"
#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <tchar.h>
#pragma warning(disable:4996)
#pragma warning(disable:4267)

using namespace std;
#define MAX 2048
map<string, string> mp;
vector<string> tagList;

CBeckyAPI bka; // You can have only one instance in a project.

HINSTANCE g_hInstance = NULL;

char szIni[_MAX_PATH + 2]; // Ini file to save your plugin settings.

/////////////////////////////////////////////////////////////////////////////
// DLL entry point
BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	g_hInstance = (HINSTANCE)hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		if (!bka.InitAPI()) {
			return FALSE;
		}
		GetModuleFileName((HINSTANCE)hModule, szIni, _MAX_PATH);
		LPSTR lpExt = strrchr(szIni, '.');
		if (lpExt) {
			strcpy(lpExt, ".ini");
		}
		else {
			// just in case
			strcat(szIni, ".ini");
		}
	}
	break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

typedef enum ByteType {
	SJIS_1st_Byte,
	SJIS_2nd_Byte,
	ASCII
} ByteType;


map<HWND, WNDPROC>	s_hWndToWindowProc;

void
SkipComment(
	char				inCommentEndMarker,
	const std::string&	inText,
	size_t				inCommentStartPos,
	size_t&				outCommentEndPos)
{
	outCommentEndPos = inCommentStartPos + 1;

	bool		isEscaped = false;
	ByteType	currentByteType = ASCII;
	for (size_t pos = inCommentStartPos + 1; pos < inText.length(); ++pos) {
		unsigned char	c = inText[pos];
		switch (currentByteType) {
		case ASCII:
		case SJIS_2nd_Byte: {
			if (0x81 <= c) {
				currentByteType = SJIS_1st_Byte;
			}
			else {
				currentByteType = ASCII;
			}
			break;
		}
		case SJIS_1st_Byte: {
			currentByteType = SJIS_2nd_Byte;
			break;
		}
		}

		if (currentByteType == ASCII && c == '\\') {
			isEscaped = !isEscaped;
		}
		else if (!isEscaped && c == inCommentEndMarker) {
			outCommentEndPos = pos;
			return;
		}
		else {
			isEscaped = false;
		}
	}
}

std::vector<std::string>
ExtractAddress(
	const std::string&	inText)
{

	//MessageBox(NULL, inText.c_str(), "ExtractAddress", MB_OK);

	std::vector<std::string>	addresses;

	bool	isEscaped = false;
	bool	hasAngleBracket = false;
	bool	isInAngleBracket = false;

	size_t	length = inText.length();



	std::string	address;
	for (size_t pos = 0; pos < length; ++pos) {

		//TCHAR msg[8];
		//wsprintf(msg, "%d", pos);
		//MessageBox(NULL, msg, "ExtractAddress", MB_OK);

		char	c = inText[pos];


		if (isEscaped) {
			isEscaped = false;
			address += c;
			continue;
		}
		else if (c == '(') {
			SkipComment(')', inText, pos, pos);
			continue;
		}
		else if (c == '\"') {
			SkipComment('\"', inText, pos, pos);
			continue;
		}
		else if (c == '\\') {
			isEscaped = !isEscaped;
			address += c;
			continue;
		}
		else if (c == '<') {
			address.clear();
			hasAngleBracket = true;
			isInAngleBracket = true;
			continue;
		}
		else if (c == '>') {
			//MessageBox(NULL, address.c_str(), "ExtractAddress", MB_OK);
			addresses.emplace_back(address);
			address.clear();
			isInAngleBracket = false;
			continue;
		}
		else if (c == ',') {
			if (!address.empty()) {
				addresses.emplace_back(address);
				address.clear();
			}
			hasAngleBracket = false;
			continue;
		}
		else if (hasAngleBracket) {
			if (isInAngleBracket) {
				address += c;
			}
		}
		else if (c != ' ' && c != 0x0A && c != 0x0D) {
			address += c;
		}
	}

	//MessageBox(NULL, address.c_str(), "ExtractAddress", MB_OK);

	if (!address.empty()) {
		TCHAR msg[8];
		wsprintf(msg, "%d", addresses.size());
		MessageBox(NULL, msg, "extractAddress", MB_OK);
		addresses.push_back(address.c_str());
		MessageBox(NULL, msg, "extractAddress", MB_OK);
		address.clear();
	}

	return(addresses);

	//std::string	addressText;
	//if (0 < addresses.size()) {
	//	addressText = addresses[0];
	//	for (size_t idx = 1; idx < addresses.size(); ++idx) {
	//		addressText += ", " + addresses[idx];
	//	}
	//}

	//return (addressText);
}

string
ConvertTagListToSingleString(
	vector<string> &tagList
	) {
	string subject;
	MessageBox(NULL, "ConvertTagListToSingleString", "debug2", MB_OK);
	if (0 < tagList.size()) {
		subject = tagList[0];
		for (size_t idx = 1; idx < tagList.size(); ++idx) {
			subject += tagList[idx];
		}
	}
	MessageBox(NULL, subject.c_str(), "debug2", MB_OK);

	return(subject);
}


bool
ExtractExistingTags(
	const string& inText)
{
	vector<string> tags;

	MessageBox(NULL, "ExtractExistingTags", "debug2", MB_OK);

	bool hasStartTag = false;

	size_t length = inText.length();
	string tag;

	bool hasTags = false;
	MessageBox(NULL, inText.c_str(), "debug2", MB_OK);

	if (inText != "") {
		return(hasTags);
	}

	for (size_t pos = 0; pos < length; ++pos) {
		char c = inText[pos];

		
		if (hasStartTag && !(c == ']')) {
			tag += c;
			continue;
		}
		else if (c == '[') {
			hasStartTag = true;
			tag += c;
			continue;
		}
		else if (c == ']') {
			hasStartTag = false;
			tag += c;
			tags.push_back(tag);
			continue;
		}
		else {
			continue;
		}
	}

	if (tags.size() != 0) {
		hasTags = true;
	}

	return(hasTags);

}




void
AddTagsToTagList(
	const string& address) {

	MessageBox(NULL, "AddTagsToTagList", "debug3", MB_OK);

	vector<string> extractedAddresses = ExtractAddress(address);
	if (0 < extractedAddresses.size()) {
		MessageBox(NULL, "AddTagsToTagList2", "debug3", MB_OK);
		tagList.push_back(mp[extractedAddresses[0]]);
		MessageBox(NULL, extractedAddresses[0].c_str(), "debug3", MB_OK);
		MessageBox(NULL, mp[extractedAddresses[0]].c_str(), "debug3", MB_OK);
		for (size_t idx = 1; idx < extractedAddresses.size(); ++idx) {
			// need fixing so that duplicated tags won't be in the list
			tagList[idx] = mp[extractedAddresses[idx]];
		}
	}
}


void
AddTagsToTagList(
	HWND hWnd,
	LPCTSTR szHeader) {
	char buffer[1024 * 64];
	string address;
	bka.CompGetSpecifiedHeader(hWnd, szHeader, buffer, sizeof(buffer));
	vector<string> extractedAddresses = ExtractAddress(buffer);
	if (0 < extractedAddresses.size()) {
		tagList[0] = mp[extractedAddresses[0]];
		for (size_t idx = 1; idx < extractedAddresses.size(); ++idx) {
			// need fixing so that duplicated tags won't be in the list
			tagList[idx] = mp[extractedAddresses[idx]];
		}
	}

}

void
SetTagsToSubject(
	HWND hWnd
	) {

	MessageBox(NULL, "SetTagsToSubject.", "debug2", MB_OK);

	char buffer[1024 * 64];
	string subject;
	bool hasTags = false;
	// retract the current subject
	bka.CompGetSpecifiedHeader(hWnd, "Subject", buffer, sizeof(buffer));
	MessageBox(NULL, buffer, "buffer", MB_OK);

	// extract already existing tagList
	hasTags = ExtractExistingTags(buffer);
	// if the subject already has tags, this plugin engs
	if (hasTags) {
		exit(0);
	}
	// convert the tagList to a single string
	subject = ConvertTagListToSingleString(tagList);
	// append the current subject
	subject += buffer;
	MessageBox(NULL, subject.c_str(), "buffer", MB_OK);
	// set the subject to the subject field
	bka.CompSetSpecifiedHeader(hWnd, "Subject", subject.c_str());

}

//#ifdef	MORE_EXTRACT
LRESULT CALLBACK
ExtractAddressWindowProc(
	HWND	hWnd,
	UINT	uMsg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	//MessageBox(NULL, "extractAddressWindowsProc", "debug2", MB_OK);

	std::map<HWND, WNDPROC>::iterator it = s_hWndToWindowProc.find(hWnd);
	if (it != s_hWndToWindowProc.end()) {
		WNDPROC	originalWindowProc = it->second;
		if (uMsg == WM_SETTEXT) {
			LPCTSTR	text = (LPCTSTR)lParam;
			AddTagsToTagList(text);
			MessageBox(NULL, "setTagsToSubject", "debug3", MB_OK);
			SetTagsToSubject(hWnd);
			return (CallWindowProc(originalWindowProc, hWnd, uMsg, wParam, lParam));
		}
		else {
			if (uMsg == WM_DESTROY) {
				s_hWndToWindowProc.erase(it);
			}
			return (CallWindowProc(originalWindowProc, hWnd, uMsg, wParam, lParam));
		}
	}
	return (0);
}

//	To,Cc,Bccのテキストボックスを探し，サブクラス化する
BOOL CALLBACK
SetupWindowProc(
	HWND	hWnd,
	LPARAM	lParam)
{
	int	ctrlID = GetDlgCtrlID(hWnd);
	switch (ctrlID) {
	case 10543:		//	To:テキストボックス
	case 10544:		//	Cc:テキストボックス
	case 10545: {	//	Bcc:テキストボックス
		WNDPROC	originalWindowProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)ExtractAddressWindowProc);
		s_hWndToWindowProc[hWnd] = originalWindowProc;
		break;
	}
	}
	return (TRUE);
}
//#endif	//	MORE_EXTRACT

/////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks from Becky!

#ifdef __cplusplus
extern "C" {
#endif

	////////////////////////////////////////////////////////////////////////
	// Called when the program is started and the main window is created.
	int WINAPI BKC_OnStart()
	{
		/*
		Since BKC_OnStart is called after Becky!'s main window is
		created, at least BKC_OnMenuInit with BKC_MENU_MAIN is called
		before BKC_OnStart. So, do not assume BKC_OnStart is called
		prior to any other callback.
		*/
		// Always return 0.

		//int i, max;

		FILE *fin;


		char buf[MAX] = { 0 };
		char *tag;
		char *address;


		if ((fin = fopen("C:\\Program Files (x86)\\RimArts\\B2\\PlugIns\\tagData.csv", "r")) == NULL) {
			MessageBox(NULL, "Cannot open the file.", "debug1", MB_OK);
			fprintf(stderr, "Cannot open the file.\n");
		}
		else {
			while (fgets(buf, MAX, fin) != NULL) {
				tag = strtok(buf, ",");
				MessageBox(NULL, tag, "debug1", MB_OK);
				address = strtok(NULL, ",");
				MessageBox(NULL, address, "debug1", MB_OK);
				mp[address] = tag;
			}
		}

		MessageBox(NULL, "tagData was read.", "debug1", MB_OK);

		//for (i + 0; i < 10; i++) {
		//	if (fgets(tag[i], 10, fin) == 0) {
		//		max = i;
		//		break;
		//	}
		//}

		//for (i = 0; i < max; i++) {
		//	printf("tag[%d] = %s", i, tag[i]);
		//}

		//fclose(fin);


		//cout << "ifstream" << endl;

		//ifstream ifs(“tagData.txt”);
		//string str;

		//if (ifs.fail()) {
		//	cerr << "File does not exits.\n";
		//	exit(0);
		//}

		//while (getline(ifs, str))
		//{
		//	sscanf(str.data(), )
		//	cout << str << endl;
		//}

		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when the main window is closing.
	int WINAPI BKC_OnExit()
	{
		// Return -1 if you don't want to quit.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when menu is intialized.
	int WINAPI BKC_OnMenuInit(HWND hWnd, HMENU hMenu, int nType)
	{
		switch (nType) {
		case BKC_MENU_MAIN:
		{
			/* Sample of adding menu items
			HMENU hSubMenu = GetSubMenu(hMenu, 4);
			// Define CmdProc as "void WINAPI CmdProc(HWND, LPARAM)"
			UINT nID = bka.RegisterCommand("Information about this Command", nType,CmdProc);
			AppendMenu(hSubMenu, MF_STRING, nID, "&Menu item");
			*/
			/* If needed, you can register the command UI update callback.
			// Define CmdUIProc as "UINT WINAPI CmdUIProc(HWND, LPARAM)"
			bka.RegisterUICallback(nID, CmdUIProc);
			*/
		}
		break;
		case BKC_MENU_LISTVIEW:
			break;
		case BKC_MENU_TREEVIEW:
			break;
		case BKC_MENU_MSGVIEW:
			break;
		case BKC_MENU_MSGEDIT:
			break;
		case BKC_MENU_TASKTRAY:
			break;
		case BKC_MENU_COMPOSE:
			break;
		case BKC_MENU_COMPEDIT:
			break;
		case BKC_MENU_COMPREF:
			break;
		default:
			break;
		}
		// Always return 0.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when a folder is opened.
	int WINAPI BKC_OnOpenFolder(LPCTSTR lpFolderID)
	{
		// Always return 0.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when a mail is selected.
	int WINAPI BKC_OnOpenMail(LPCTSTR lpMailID)
	{
		// Always return 0.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called every minute.
	int WINAPI BKC_OnEveryMinute()
	{
		// Always return 0.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when a compose windows is opened.
	int WINAPI BKC_OnOpenCompose(HWND hWnd, int nMode/* See COMPOSE_MODE_* in BeckyApi.h */)
	{
		if (nMode != COMPOSE_MODE_FORWARD2) {
			AddTagsToTagList(hWnd, "To");
			AddTagsToTagList(hWnd, "Cc");
			AddTagsToTagList(hWnd, "Bcc");
		}
		//#ifdef	MORE_EXTRACT
			//	ウィンドウ内のコントロールからTo,Cc,Bccのテキストボックスを探し，サブクラス化する
		EnumChildWindows(hWnd, SetupWindowProc, 0);
		//#endif

			// Always return 0.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when the composing message is saved.
	int WINAPI BKC_OnOutgoing(HWND hWnd, int nMode/* 0:SaveToOutbox, 1:SaveToDraft, 2:SaveToReminder*/)
	{
		// Return -1 if you do not want to send it yet.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when a key is pressed.
	int WINAPI BKC_OnKeyDispatch(HWND hWnd, int nKey/* virtual key code */, int nShift/* Shift state. 0x40=Shift, 0x20=Ctrl, 0x60=Shift+Ctrl, 0xfe=Alt*/)
	{
		// Return TRUE if you want to suppress subsequent command associated to this key.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when a message is retrieved and saved to a folder
	int WINAPI BKC_OnRetrieve(LPCTSTR lpMessage/* Message source*/, LPCTSTR lpMailID/* Mail ID*/)
	{
		// Always return 0.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when a message is spooled
	int WINAPI BKC_OnSend(LPCTSTR lpMessage/* Message source */)
	{
		// Return BKC_ONSEND_PROCESSED, if you have processed this message
		// and don't need Becky! to send it.
		// Becky! will move this message to Sent box when the sending
		// operation is done.
		// CAUTION: You are responsible for the destination of this
		// message if you return BKC_ONSEND_PROCESSED.

		// Return BKC_ONSEND_ERROR, if you want to cancel the sending operation.
		// You are responsible for displaying an error message.

		// Return 0 to proceed the sending operation.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when all messages are retrieved
	int WINAPI BKC_OnFinishRetrieve(int nNumber/* Number of messages*/)
	{
		// Always return 0.
		return 0;
	}


	////////////////////////////////////////////////////////////////////////
	// Called when plug-in setup is needed.
	int WINAPI BKC_OnPlugInSetup(HWND hWnd)
	{
		// Return nonzero if you have processed.
		// return 1;
		return 0;
	}


	////////////////////////////////////////////////////////////////////////
	// Called when plug-in information is being retrieved.
	typedef struct tagBKPLUGININFO
	{
		char szPlugInName[80]; // Name of the plug-in
		char szVendor[80]; // Name of the vendor
		char szVersion[80]; // Version string
		char szDescription[256]; // Short description about this plugin
	} BKPLUGININFO, *LPBKPLUGININFO;

	int WINAPI BKC_OnPlugInInfo(LPBKPLUGININFO lpPlugInInfo)
	{
		/* You MUST specify at least szPlugInName and szVendor.
		   otherwise Becky! will silently ignore your plug-in.
		   */
		strcpy(lpPlugInInfo->szPlugInName, "Auto Subject Tag PlugIn");
		strcpy(lpPlugInInfo->szVendor, "Teruhisa Maruya");
		strcpy(lpPlugInInfo->szVersion, "1.0");
		strcpy(lpPlugInInfo->szDescription, "Enables auto-subject tag insersion.");

		// Always return 0.
		return 0;
	}

	////////////////////////////////////////////////////////////////////////
	// Called when drag and drop operation occurs.
	int WINAPI BKC_OnDragDrop(LPCSTR lpTgt, LPCSTR lpSrc, int nCount, int dropEffect)
	{
		/*
		lpTgt:	A folder ID of the target folder.
				You can assume it is a root mailbox, if the string
				contains only one '\' character.
		lpSrc:	Either a folder ID or mail IDs. Multiple mail IDs are
				separated by '\n' (0x0a).
				You can assume it is a folder ID, if the string
				doesn't contain '?' character.
		nCount:	Number of items to be dropped.
				It can be more than one, if you drop mail items.
		dropEffect: Type of drag and drop operation
				1: Copy
				2: Move
				4: Link (Used for filtering setup in Becky!)
		*/
		// If you want to cancel the default drag and drop action,
		// return -1;
		// Do not assume the default action (copy, move, etc.) is always
		// processed, because other plug-ins might cancel the operation.
		return 0;
	}


	////////////////////////////////////////////////////////////////////////
	// Called when a message was retrieved and about to be filtered.
	int WINAPI BKC_OnBeforeFilter2(LPCSTR lpMessage, LPCSTR lpMailBox, int* lpnAction, char** lppParam)
	{
		/*
		lpMessage: A complete message source, which ends with
		"<CRLF>.<CRLF>".
		lpnAction:	[out] Returns the filtering action to be applied.
			#define ACTION_NOTHING		-1	// Do nothing
			#define ACTION_MOVEFOLDER	0	// Move to a folder
			#define ACTION_COLORLABEL	1	// Set the color label
			#define ACTION_SETFLAG		2	// Set the flag
			#define ACTION_SOUND		3	// Make a sound
			#define ACTION_RUNEXE		4	// Run executable file
			#define ACTION_REPLY		5	// Reply to the message
			#define ACTION_FORWARD		6	// Forward the message
			#define ACTION_LEAVESERVER	7	// Leave/delete on the server.
			#define ACTION_ADDHEADER	8	// Add "X-" header to the top of the message. (Plug-in only feature)
		lpMailBox: ID of the mailbox that is retrieving the message. (XXXXXXXX.mb\)
		lppParam:	[out] Returns the pointer to the filtering parameter string.
			ACTION_MOVEFOLDER:	Folder name.
								e.g. XXXXXXXX.mb\FolderName\
								or \FolderName\ (begin with '\') to use
								the mailbox the message belongs.
			ACTION_COLORLABEL:	Color code(BGR) in hexadecimal. e.g. 0088FF
			ACTION_SETFLAG:		"F" to set flag. "R" to set read.
			ACTION_SOUND:		Name of the sound file.
			ACTION_RUNEXE:		Command line to execute. %1 will be replaced with the path to the file that contains the entire message.
			ACTION_REPLY:		Path to the template file without extension.
									e.g. #Reply\MyReply
			ACTION_FORWARD:		Path to the template file without extension. + "*" + Address to forward.
									e.g. #Forward\MyForward*mail@address
										 *mail@address (no template)
			ACTION_LEAVESERVER:	The string consists of one or two decimals. The second decimal is optional.
								Two decimals must be separated with a space.
								First decimal	1: Delete the message from the server.
												0: Leave the message on the server.
								Second decimal	1: Do not store the message to the folder.
												0: Store the message to the folder. (default action)
								e.g. 0 (Leave the message on the server.)
									 1 1 (Delete the message on the server and do not save. (Means KILL))
			ACTION_ADDHEADER	"X-Header:data" that will be added at the top of the incoming message.
								You can specify multiple headers by separating CRLF, but each header must
								begin with "X-". e.g. "X-Plugindata1: test\r\nX-Plugindata2: test2";
		*/

		/* Return values
		BKC_FILTER_DEFAULT	Do nothing and apply default filtering rules.
		BKC_FILTER_PASS		Apply default filtering rules after applying the rule it returns.
		BKC_FILTER_DONE		Do not apply default rules.
		BKC_FILTER_NEXT		Request Becky! to call this callback again so that another rules can be added.
		*/
		return BKC_FILTER_DEFAULT;
	}

#ifdef __cplusplus
}
#endif
