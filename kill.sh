kill $(ps aux | grep webserv | awk '{printf("%d ", $2)}')
