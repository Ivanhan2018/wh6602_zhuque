:functionstops

rem 6602½ð±Ò

	call taskkill /im ServiceLoader.exe /t /f 
	call taskkill /im ControlServer.exe /t /f 
	call taskkill /im LogonServer.exe /t /f 
	call taskkill /im CenterServer.exe /t /f 

goto :eof