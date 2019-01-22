#include "TelnetSession.h"
#include "TelnetServer.h"
#include "IHarbor.h"
#define TELNET_CMD_DEL								"\033[K"					//删除字符
#define TELNET_CMD_UP								"\033[A"					//上
#define TELNET_CMD_DOWN								"\033[B"					//下
#define TELNET_CMD_RIGHT							"\033[C"					//右
#define TELNET_CMD_LEFT								"\033[D"					//左
#define TELNET_CMD_HOME								"\033[1~"					//HOME
#define TELNET_CMD_END								"\033[4~"					//END

#define TELNET_CMD_MOVE_FOCUS_LEFT_MAX				"\33[999999999D"			//左移光标至最左端
#define PROTOCOL_WILL_GOHEAD						"\377\373\003"				// WILL GOHEAD
#define PROTOCOL_DO_GOHEAD							"\377\375\003"				// DO GOHEAD
#define PROTOCOL_WONT_ECHO							"\377\374\001"				// WON'T ECHO
#define PROTOCOL_WILL_ECHO							"\377\373\001"				// WILL ECHO
#define PROTOCOL_DO_ECHO							"\377\375\001"				// DO ECHO

sl::SLPool<TelnetSession> TelnetSession::s_pool;
int32 TelnetSession::onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen){
	int32 idx = 0;
	while(idx < dwLen){
		char c = pContext[idx++];
		switch(c){
		case '\003': //close socket
			close();
			break;

		case '\377':{
			std::string s = "";
			s += c;
			while(idx < dwLen){
				s += pContext[idx++];
			}
			if(s.find(PROTOCOL_DO_GOHEAD) != string::npos){
				printf("set client do gohead success\n");
			}
			if(s.find(PROTOCOL_DO_ECHO) != string::npos){
				printf("set client do echo success\n");
			}
			break;
		}
		case '\r':
			if(idx < dwLen){
				char cc = pContext[idx++];
				if(cc == '\0'){
					if(!processCommand())
						return dwLen;
				}
			}
			break;
		case 8: //退格
			if(_curPos == 0){
				resetStartPosition();
			}else{
				sendDelChar();
				checkAfterStr();
			}
			break;
		case 27:{
			std::string s = "";
			std::string vt100cmd(s+c);
			bool beContinue = true;
			
			while(beContinue && idx < dwLen){
				if(pContext[idx] == '\r')
					break;

				c = pContext[idx++];
				vt100cmd.append(s+c);
				switch (c){
				case 'A': //光标上移n行
				case 'B': //光标下移n行
				case 'C': //光标右移n列
				case 'D': //光标左移n列
				case '~': //home、end
					beContinue = false;
					break;
				
				case 'm': //颜色等属性或命令
				case 'J': //清屏
				case 'K': //清除光标到行尾的内容
				case 's': //保存光标位置
				case 'u': //恢复光标位置
				case 'l': //影藏光标
				case 'h': //显示光标
				case 'H': //设置光标位置
				default:
					break;
				}
			}

			if(!checkUDLR(vt100cmd)){
				vt100cmd[0] = '^';
				_command.insert(_curPos, vt100cmd);
				_curPos += vt100cmd.length();
				if(_curPos == (int32)_command.length()){
					send(vt100cmd.c_str(), vt100cmd.size());
				}
				else{
					std::string s = _command.substr(_curPos - vt100cmd.length(), _command.size() - _curPos + vt100cmd.length());
					char tail[32] = {0};
					SafeSprintf(tail, sizeof(tail), "\33[%dD", (int32)_command.size() - _curPos);
					send(tail, strlen(tail));
				}
			}
			break;
		}
		case 0x7f: //delete
			if(_curPos < (int32)_command.length()){
				_command.erase(_curPos, 1);
				send(TELNET_CMD_DEL, strlen(TELNET_CMD_DEL));
				checkAfterStr();
			}
			break;
		default:{
				std::string s = "";
				s += c;
				_command.insert(_curPos, s);
				_curPos++;
				sendClientEcho(s);
				checkAfterStr();
				break;
			}
		}
	}
	return dwLen;
}

void TelnetSession::send(const void* pContext, const int32 size){
	ITcpSession::send(pContext, size); 
}

void TelnetSession::onConnected(sl::api::IKernel* pKernel){
	setWillGoHead();
	setWillEcho();
	std::string welcomeStr = getWelcome();
	send(welcomeStr.c_str(), welcomeStr.size());
}

void TelnetSession::onDisconnect(sl::api::IKernel* pKernel){
	release();
}

void TelnetSession::resetStartPosition(){
	send(TELNET_CMD_MOVE_FOCUS_LEFT_MAX, strlen(TELNET_CMD_MOVE_FOCUS_LEFT_MAX));
	std::string startStr = getInputStartString();
	char backCmd[32] = {0};
	SafeSprintf(backCmd, sizeof(backCmd), "\33[%dC", startStr.size());
	send(backCmd, strlen(backCmd));
}

void TelnetSession::sendClientEcho(const std::string& s){
	send(s.c_str(), s.size());
	/*char tail[32] = {0};
	SafeSprintf(tail, sizeof(tail), "\33[%dC", s.size());
	send("\33[1C", strlen("\33[1C"));*/
}

void TelnetSession::setWontEcho(){
	send(PROTOCOL_WONT_ECHO,strlen(PROTOCOL_WONT_ECHO));
}

void TelnetSession::setWillEcho(){
	send(PROTOCOL_WILL_ECHO,strlen(PROTOCOL_WILL_ECHO));
}

void TelnetSession::setWillGoHead(){
	send(PROTOCOL_WILL_GOHEAD, strlen(PROTOCOL_WILL_GOHEAD));
}

bool TelnetSession::checkUDLR(const std::string& cmd){
	if(cmd.find(TELNET_CMD_UP) != std::string::npos){  //上
		send(TELNET_CMD_MOVE_FOCUS_LEFT_MAX, strlen(TELNET_CMD_MOVE_FOCUS_LEFT_MAX));
		sendDelChar();
		std::string startStr = getInputStartString();
		send(startStr.c_str(), startStr.size());
		resetStartPosition();

		std::string s = getHistoryCommand(false);
		send(s.c_str(), s.size());
		_command = s;
		_curPos = s.size();
		return true;
	}
	else if(cmd.find(TELNET_CMD_DOWN) != std::string::npos){
		send(TELNET_CMD_MOVE_FOCUS_LEFT_MAX, strlen(TELNET_CMD_MOVE_FOCUS_LEFT_MAX));
		sendDelChar();
		std::string startStr = getInputStartString();
		send(startStr.c_str(), startStr.size());
		resetStartPosition();

		std::string s = getHistoryCommand(true);
		send(s.c_str(), s.size());
		_command = s;
		_curPos = s.size();
		return true;
	}
	else if(cmd.find(TELNET_CMD_RIGHT) != std::string::npos){
		if(_curPos < _command.size()){
			_curPos++;
			send(TELNET_CMD_RIGHT, strlen(TELNET_CMD_RIGHT));
		}
		return true;
	}
	else if(cmd.find(TELNET_CMD_LEFT) != std::string::npos){
		if(_curPos > 0){
			_curPos--;
			send(TELNET_CMD_LEFT, strlen(TELNET_CMD_LEFT));
		}
		return true;
	}
	else if(cmd.find(TELNET_CMD_HOME) != std::string::npos){
		if(_curPos > 0){
			char cmdStr[32] = {0};
			SafeSprintf(cmdStr, sizeof(cmdStr), "\033[%dD", (int32)_curPos);
			send(cmdStr, strlen(cmdStr));
			_curPos = 0;
		}
		return true;
	}
	else if(cmd.find(TELNET_CMD_END) != std::string::npos){
		if(_curPos != _command.length()){
			char cmdStr[32] = {0};
			SafeSprintf(cmdStr, sizeof(cmdStr), "\033[%dC", _command.length() - _curPos);
			send(cmdStr, strlen(cmdStr));
			_curPos = _command.length();
		}
		return true;
	}
	return false;
}

std::string TelnetSession::getInputStartString(){
	return ">>>";
}

std::string TelnetSession::getWelcome(){
	char welcomeStr[1024] = {0};
	SafeSprintf(welcomeStr, sizeof(welcomeStr), "Welcome to %s %d\r\nBuilt: %s %s\r\n%s", SLMODULE(Harbor)->getNodeTypeStr(), SLMODULE(Harbor)->getNodeId(), __TIME__, __DATE__, getInputStartString().c_str());
	return welcomeStr;
}

void TelnetSession::sendDelChar(){
	if(_command.size() <= 0 || _curPos <= 0)
		return;

	_command.erase(_curPos -1, 1);
	_curPos--;
	
	static const char* moveFocus = "\33[1D";
	send(moveFocus, strlen(moveFocus));

	send(TELNET_CMD_DEL, strlen(TELNET_CMD_DEL));
}

void TelnetSession::checkAfterStr(){
	if(_curPos != _command.size()){
		std::string s = "";
		s = _command.substr(_curPos, _command.size() - _curPos);

		char tail[32] = {0};
		SafeSprintf(tail, sizeof(tail), "\33[%dD", (int32)s.size());
		std::string tailStr = tail;
		s += tailStr; 

		send(s.c_str(), s.size());
	}
}

void TelnetSession::sendEnter(){
	static const char* newLine = "\r\n";
	send(newLine, strlen(newLine));
}

void TelnetSession::sendNewLine(){
	sendEnter();
	std::string startStr = getInputStartString();
	send(startStr.c_str(), startStr.size());
	resetStartPosition();
	_curPos = 0;
}

void TelnetSession::historyCommandCheck(){
	if(_historyCommand.size() > 50)
		_historyCommand.pop_front();

	if(_historyCommandIndex < 0)
		_historyCommandIndex = _historyCommand.size() -1;

	if(_historyCommandIndex > _historyCommand.size() -1)
		_historyCommandIndex = 0;
}

bool TelnetSession::processCommand(){
	if(_command.size() == 0){
		sendNewLine();
		return true;
	}

	_historyCommand.push_back(_command);
	historyCommandCheck();
	_historyCommandIndex = _historyCommand.size() -1;

	bool ret = true;
	ITelnetHandler* handler = _telnetServer->findTelnetHandler(_handlerName.c_str());
	if(handler){
		std::string retBuf;
		ret = handler->processTelnetCommand(_command, retBuf);

		std::string::size_type pos = 0;
		while((pos = retBuf.find('\n', pos)) != std::string::npos){
			if(retBuf[pos -1] != '\r'){
				retBuf.insert(pos, "\r");
			}
			pos++;
		}

		if(retBuf.size() > 0){
			sendEnter();
			send(retBuf.c_str(), retBuf.size());
		}
	}

	_command = "";
	sendNewLine();
	return ret;
}

std::string TelnetSession::getHistoryCommand(bool isNextCommand){
	if(isNextCommand)
		_historyCommandIndex++;
	else
		_historyCommandIndex--;

	historyCommandCheck();

	if(_historyCommand.size() == 0)
		return "";

	return _historyCommand[_historyCommandIndex];
}
