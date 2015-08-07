#!/bin/sh

#���з������Ĺ�����

#����������Ĳ���װ·��
QMONSTER_RUNTIME_ENV_PATH=/home/wulinonlinetest/RuntimeEnv

#���°��ڷ������ϴ�ŵ�·��
QMONSTER_UPDATE_PACKAGE_PATH=/data/qmonster/Packages

#�ű�ʹ�ð����ĵ�
Usage()
{
	echo "Usage: ./ServerAdminTools.sh -I <SERVERIP> <ACCOUNT_PORT> <ZONE_PORT> <REGAUTH_PORT> <WORLDID> <ZONEID> <DBUser> <DBPasswd>"
	echo "                        --Init Server Project"
	echo "       ./ServerAdminTools.sh -D <PackageName>"
	echo "                        --Deploy Server update package to runtime env"
	echo "       ./ServerAdminTools.sh -K"
	echo "                        --Kill All Servers of local env"
	echo "       ./ServerAdminTools.sh -S"
	echo "                        --Restart All Servers of local env"

	echo "       ./ServerAdminTools.sh -R"
	echo "                        --Reload All Servers of local env"

	exit 1
}

#��ʼ������Server
Init_Server()
{
	if [ $# -ne 8 ]
	then
		echo "Usage: ./ServerAdminTools.sh -I <SERVERIP> <ACCOUNT_PORT> <ZONE_PORT> <REGAUTH_PORT> <WORLDID> <ZONEID> <DBUser> <DBPasswd>"
		exit 1
	fi

	#���ԭ��������ʱ�������ڣ������
	if [ -d "$QMONSTER_RUNTIME_ENV_PATH" ]
	then
		rm -rf $QMONSTER_RUNTIME_ENV_PATH
	fi

	#��������ʱ����
	mkdir -p $QMONSTER_RUNTIME_ENV_PATH

	cp ../* $QMONSTER_RUNTIME_ENV_PATH/ -rf

	cd $QMONSTER_RUNTIME_ENV_PATH/
	
	#����AccountGatewayServer�󶨵ķ�����IP
	perl -i -p -e 's/'#ACCOUNT_GATEWAY_IP#'/'$1'/g' World/AccountGatewayServer/conf/LotusListener.tcm

	#����AccountGatewayServer�󶨵ķ������˿�
	perl -i -p -e 's/'#ACCOUNT_GATEWAY_PORT#'/'$2'/g' World/AccountGatewayServer/conf/LotusListener.tcm

	#����ZoneGatewayServer�󶨵ķ�����IP
	perl -i -p -e 's/'#ZONE_GATEWAY_IP#'/'$1'/g' Zone/ZoneGatewayServer/conf/LotusListener.tcm

	#����ZoneGatewayServer�󶨵ķ������˿�
	perl -i -p -e 's/'#ZONE_GATEWAY_PORT#'/'$3'/g' Zone/ZoneGatewayServer/conf/LotusListener.tcm

	#����RegAuthGatewayServer�󶨵ķ�����IP
	perl -i -p -e 's/'#REGAUTH_GATEWAY_IP#'/'$1'/g' Global/RegAuthGatewayServer/conf/LotusListener.tcm
	
	#����RegAuthGatewayServer�󶨵ķ������˿�
	perl -i -p -e 's/'#REGAUTH_GATEWAY_PORT#'/'$4'/g' Global/RegAuthGatewayServer/conf/LotusListener.tcm

	#�������������ļ��еķ�����ͨ��BUS
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' Zone/ZoneServer/conf/GameServer.tcm World/AccountServer/conf/GameServer.tcm World/RoleDBServer/conf/GameServer.tcm World/WorldServer/conf/GameServer.tcm World/NameDBServer/conf/GameServer.tcm

	perl -i -p -e 's/'#ZONEID#'/'$6'/g' Zone/ZoneServer/conf/GameServer.tcm World/WorldServer/conf/GameServer.tcm

	perl -i -p -e 's/'#SERVERIP#'/'$1'/g' Zone/ZoneServer/conf/GameServer.tcm Global/RegAuthServer/conf/GameServer.tcm Global/AccountDBServer/conf/GameServer.tcm World/AccountServer/conf/GameServer.tcm World/RoleDBServer/conf/GameServer.tcm World/WorldServer/conf/GameServer.tcm World/NameDBServer/conf/GameServer.tcm

	#������Ϸʹ��DB����ϸ��Ϣ
	perl -i -p -e 's/'#DB_IP#'/'$1'/g' World/RoleDBServer/conf/DBMSConf.xml World/NameDBServer/conf/DBMSConf.xml Global/AccountDBServer/conf/DBMSConf_UniqUin.xml Global/AccountDBServer/conf/DBMSConf.xml
	perl -i -p -e 's/'#DB_USER#'/'$7'/g' World/RoleDBServer/conf/DBMSConf.xml World/NameDBServer/conf/DBMSConf.xml Global/AccountDBServer/conf/DBMSConf_UniqUin.xml Global/AccountDBServer/conf/DBMSConf.xml
	perl -i -p -e 's/'#DB_PASSWD#'/'$8'/g' World/RoleDBServer/conf/DBMSConf.xml World/NameDBServer/conf/DBMSConf.xml Global/AccountDBServer/conf/DBMSConf_UniqUin.xml Global/AccountDBServer/conf/DBMSConf.xml
					
	#World�е����з�����������WorldServer/conf���棬����Ҫ�ֶ����õ�

	#�������������ű�����������
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/AccountServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/RoleDBServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/WorldServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' Zone/ZoneServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#ZONEID#'/'$6'/g' Zone/ZoneServer/admin/ScriptFrameSvr.sh
	perl -i -p -e 's/'#WORLDID#'/'$5'/g' World/NameDBServer/admin/ScriptFrameSvr.sh	

	echo "Success to Init Server development env......."
	echo "Have a good time......"
	echo "Finished!"

	exit 1
}

#���²��������
Deploy_Server()
{
	#��ͣ�����е�Server
	Kill_Server
	
	#ֱ�ӽ���װ����ѹ����ӦĿ¼����
	cd $QMONSTER_UPDATE_PACKAGE_PATH
	
	
	tar -zxvf $1 -C $QMONSTER_RUNTIME_ENV_PATH --exclude LotusListener.tcm --exclude *svn* --exclude log --exclude conf --exclude TCP* --exclude ScriptFrameSvr* --exclude bills --exclude admin

	#�������еķ���
	Start_Server

	echo "Success to deploy server runtime env!!!"

	exit 0	
}

#ͣ�����е�Server
Kill_Server()
{
	#ֹͣ���еķ���
	cd $QMONSTER_RUNTIME_ENV_PATH

	#ֹͣAccountGatewayServer
	cd World/AccountGatewayServer/admin
	./stop.sh
	cd ../../../

	#ֹͣAccountServer
	cd World/AccountServer/admin
	./stop.sh
	cd ../../../

	#ֹͣZoneGatewayServer
	cd Zone/ZoneGatewayServer/admin
	./stop.sh
	cd ../../..

	#ֹͣZoneServer
	cd Zone/ZoneServer/admin
	./stop.sh
	cd ../../..

	sleep 8

	#ֹͣWorldServer
	cd World/WorldServer/admin
	./stop.sh
	cd ../../../

	#ֹͣRoleDBServer
	cd World/RoleDBServer/admin
	./stop.sh
	cd ../../../

	#ֹͣNameDBServer
	cd World/NameDBServer/admin
	./stop.sh
	cd ../../../

	#ȥ��Cluster
	#ֹͣClusterServer
	#cd Cluster/ClusterServer/admin
	#./stop.sh
	#cd ../../..

	#ֹͣRegAuthGatewayServer
	cd Global/RegAuthGatewayServer/admin
	./stop.sh
	cd ../../..

	#ֹͣRegAuthServer
	cd Global/RegAuthServer/admin
	./stop.sh
	cd ../../..

	#ֹͣAccountDBServer
	cd Global/AccountDBServer/admin
	./stop.sh
	cd ../../..

	echo "Success to stop all servers!!!"
}

#�������еķ�����
#todo jasonxiong ������Ҫ�����ܽ��з������ķ�������
Start_Server()
{
	#�����������еķ���
	cd $QMONSTER_RUNTIME_ENV_PATH

	#����AccountGatewayServer
	cd World/AccountGatewayServer/admin
	./restart.sh
	cd ../../../

	#����AccountServer
	cd World/AccountServer/admin
	./restart.sh
	cd ../../../

	#����WorldServer
	cd World/WorldServer/admin
	./restart.sh
	cd ../../../

	#����RoleDBServer
	cd World/RoleDBServer/admin
	./restart.sh
	cd ../../../

	#����NameDBServer
	cd World/NameDBServer/admin
	./restart.sh
	cd ../../../

	#����ZoneGatewayServer
	cd Zone/ZoneGatewayServer/admin
	./restart.sh
	cd ../../..

	#����ZoneServer
	cd Zone/ZoneServer/admin
	./restart.sh
	cd ../../..

	#ȥ��Cluster
	#����ClusterServer
	#cd Cluster/ClusterServer/admin
	#./restart.sh
	#cd ../../..

	#����RegAuthGatewayServer
	cd Global/RegAuthGatewayServer/admin
	./restart.sh
	cd ../../..
	
	#����RegAuthServer
	cd Global/RegAuthServer/admin
	./restart.sh
	cd ../../..

	#����AccountDBServer
	cd Global/AccountDBServer/admin
	./restart.sh
	cd ../../..

	echo "Success to restart all servers!!!"
}

#Reload���еķ�����
Reload_Server()
{
	#Reload���еķ�������
	cd $QMONSTER_RUNTIME_ENV_PATH

	#���¼���AccountGatewayServer
	cd World/AccountGatewayServer/admin
	./reload.sh
	cd ../../../

	#���¼���AccountServer
	cd World/AccountServer/admin
	./reload.sh
	cd ../../../

	#���¼���WorldServer
	cd World/WorldServer/admin
	./reload.sh
	cd ../../../

	#���¼���RoleDBServer
	cd World/RoleDBServer/admin
	./reload.sh
	cd ../../../

	#���¼���NameDBServer
	cd World/NameDBServer/admin
	./reload.sh
	cd ../../../

	#���¼���ZoneGatewayServer
	cd Zone/ZoneGatewayServer/admin
	./reload.sh
	cd ../../..

	#���¼���ZoneServer
	cd Zone/ZoneServer/admin
	./reload.sh
	cd ../../..

	#ȥ��Cluster
	#���¼���ClusterServer
	#cd Cluster/ClusterServer/admin
	#./reload.sh
	#cd ../../..

	#���¼���RegAuthGatewayServer
	cd Global/RegAuthGatewayServer/admin
	./reload.sh
	cd ../../..
	
	#���¼���RegAuthServer
	cd Global/RegAuthServer/admin
	./reload.sh
	cd ../../..

	#���¼���AccountDBServer
	cd Global/AccountDBServer/admin
	./reload.sh
	cd ../../..

	echo "Success to reload all servers!!!"
}

#�ű���ѭ���������������������ִ�еĽű�����

while getopts ":I:D:KSR" options
do
	case $options in
		I)
			if [ $# -ne 9 ]
			then
				Usage

				exit 1
			fi

			Init_Server $2 $3 $4 $5 $6 $7 $8 $9

			exit 1
			;;

		D)
			if [ $# -ne 2 ]
			then
				Usage
				
				exit 1
			fi
			
			Deploy_Server $2
			
			exit 1
			;;
			
		K)
			Kill_Server
		
			exit 1
			;;

		S)
			Start_Server
		
			exit 1;;

		R)
			Reload_Server

			exit 1
			;;

		*)
			Usage

			exit 1
			;;

		esac

done

Usage
