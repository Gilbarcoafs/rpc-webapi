set IP=192.168.2.119
set ssh_param=-o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no

@echo *****************
@echo *** Deploying ***
@echo *****************

:: kill any old instances
ssh %ssh_param% pi@%IP% "pkill \"rpc\\+webapi\""

:: copy the webapi to the rpi
@scp %ssh_param% ../rpc+webapi pi@%IP%:~/

:: start the webapi
@ssh %ssh_param% pi@%IP% "chmod +x rpc+webapi; ./rpc+webapi"
