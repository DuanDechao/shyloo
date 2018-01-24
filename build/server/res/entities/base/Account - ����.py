# -*- coding: utf-8 -*-

# ------------------------------------------------------------------------------
# Section: class Account
# 断开连接与destroy都设置了timeout延后执行，主要是不想entity被频繁的load/save
# ------------------------------------------------------------------------------

# engine
import BigWorld
import Netease
import collections
import json

# inner
import properties
import const
import bsql
import bwdecorator
import switches
import strings
from data_reader import reader
from utils import safesuper
from utils import isDigit
from utils import isAlpha
import utils
from gm import GMAdmin
import BWEngine
import BWConfigs
import mask_words
import time
from ProxyMeta import ProxyMeta
import GetUsedNames
# inherit
from interfaces.iProxy import iProxyNoCell
from interfaces.iQueue import iQueue
from interfaces.iActiveCode import iActiveCode
from interfaces.iAccountGlobal import iAccountGlobal
from log import CreateRoleLog
from utils import Time

LOGONATTEMPT_CD = 3
LAST_SESSIONS_QUEUE_SIZE = 3
class Account(iProxyNoCell, iQueue, iActiveCode, iAccountGlobal):
	__metaclass__ = ProxyMeta
	ignoreDBCall = False
	def __init__(self):
		super(Account, self).__init__()
		self.oldsession = ''
		self.session = ''
		self.logger.info("Account __init__")
		self.playerGID = 0
		self.client_ip = None
		self.currentGID = 0
		self.reservation = BWConfigs.isReservation() and 1 or 0
		self.pushToken = ''

		if not self.databaseID and not self.ignoreDBCall:
			self.registTimer(0, "onTimerWriteDB", exclude=True)

	def onTimerWriteDB(self, timerid, userdata):
		self.writeToDB()
		import UserInfo 

		v = self.playerName.rsplit('@', 1)
		cdomain = v[-1] if len(v)==2 else ""
		UserInfo.UserFromInfo.onNewCreation(
				aid = self.aid, 
				aidtime=self.newtm, 
				cdomain=cdomain,
				urs=self.playerName,
				hostid=BWConfigs.getHostID(),
				channel=self.channel,
				platform=self.platform, 
				package_type=self.package_type,
				login_type=self.login_type,
				callback=self._onQueryNewCreation
				)

	def _onQueryNewCreation(self, num, ret, error):
		self.logger.info('_onQueryNewCreation', num=num, ret=ret, error=error)
		if error:
			self.logger.error('_onQueryNewCreation fail. ', error=error)
		else:
			#{u'0': {u'hostid': u'131031634', u'newtm': 1472026469, u'timestamp': datetime.datetime(2017, 5, 19, 20, 26, 20), u'package_type': u'yiyou', u'platform': u'pc', u'_id': ObjectId('591ee46dec960881b0f64303')}}
			ret = ret['0']
			self.firstChannel.hostid = str(ret['hostid'])
			self.firstChannel.newtm = int(ret['newtm'])
			self.firstChannel.package_type = str(ret['package_type'])
			self.firstChannel.login_type = int(ret['login_type'])
			self.firstChannel.channel = str(ret['channel'])

	@bwdecorator.ggs_exposed
	@bwdecorator.keyrpc
	def handshake(self, transferFromServer, session):
		super(Account, self).handshake()
		self.oldsession, self.session = self.session, session 

		if self.destroyTimerID:
			self.cancelTimer(self.destroyTimerID)
			self.destroyTimerID = 0

		if BWConfigs.isLGS():
			self.handshake4Normal()
		elif BWConfigs.isGGS():
			self.handshake4GGS()
		else:
			raise NotImplementedError

	def handshake4Normal(self):
		if not self.reservation:
			self.createAvatar()
		else:
			if self.reserved:
				self.safeClient.onReserved(1, self._reservationPromote())
			else:
				BWEngine.getGlobalBase("QueueStub").checkReservation(self, 'createAvatar', ())

	def onRelogin(self):
		super(Account, self).onRelogin()
		if BWConfigs.isLGS():
			peer = getattr(self.nextProxy, 'peer', None)
			peer and peer.onRelogin()

	@bwdecorator.ggs_exposed
	def setClientConfig(self, clientConf):
		self.clientConfig = clientConf

		if self.nextProxy:
			self.nextProxy.setClientConfig(self.clientConfig)

	def createAvatar(self):
		if self.nextProxy:
			#self.onRelogin()
			self.loadSceneAfterReconnect()
		## 恢复的玩家直接进
		#elif self.recoveringGID:
		#	self._hasAvatar()
		elif getattr(self, "inGame", False) != True:
			#检查激活情况
			self.queryAccountActivateStatusRequest()
		else:
			#已经排队完成，但还未连接client
			self._hasAvatar()

	def loadSceneAfterReconnect(self):
		self.nextProxy.loadSceneAfterReconnect(self)

	def onRecoverFromDisconnect(self):
		if self.reservation:
			try:
				self.nextProxy.entireDestroy(False, True)
			finally:
				self.safeClient.onReserved(1, self._reservationPromote())
				self.disconnectByServer(delay=3)
		elif self.hasClient:
			self.giveClientTo(self.curProxy)
			super(Account, self).onRecoverFromDisconnect()

	def _checkUserName(self):
		#长度限制
		#MAX_LEN = 7 * 3
		#MIN_LEN = 2 * 3
		code = utils.checkUserName(self.userName)
		if code != const.UserNameCheckCode.RET_OK:
			self.onSelectCharacter(code)
			return

		#查重
		sql = bsql.SQL_HAS_USERNAME % Netease.mysqlEscape(self.userName)
		BigWorld.executeRawDatabaseCommand(sql, self._selectCharacterCallback)
	
	def _reservationPromote(self):
		promote = '''캐릭터 생성 예약에 성공했습니다. 【%s】%s월 %s일 %s시 서버 정식 오픈 예정입니다. 많은 기대 부탁드립니다.'''
		t = BWConfigs.getServerOpenTime()
		if not t:
			t = 1515233656 # magic num
		ts = time.localtime(t) 
		return promote%(BWConfigs.getServerName(), ts.tm_mon, ts.tm_mday, ts.tm_hour)

	@bwdecorator.keyrpc
	def selectCharacter(self, charType, userName, pushToken):
		print "select character:%s  %s"%(charType, userName)
		if not self.checkCreateNext():
			self.logger.error("[lb] Account.selectCharacter while creating next.")
			return

		if self.reservation:
			if self.resultSet:
				self.safeClient.onReserved(1, self._reservationPromote())
				self.disconnectByServer(delay=3)
			else:
				BWEngine.getGlobalBase("QueueStub").checkReservation(self, 'realSelectCharacter', (charType, userName, pushToken))
		else:
			self.realSelectCharacter(charType, userName, pushToken)
	
	def onCheckReservation(self, full, callback, args):
		if full:
			self.safeClient.onReserved(0, "이 서버 캐릭터 생성 예약은 가득 찼습니다. 다른 서버를 선택하거나 정식 오픈까지 기다려주세요.")
			self.disconnectByServer(delay=3)
		else:
			getattr(self, callback)(*args)
	
	def realSelectCharacter(self, charType, userName, pushToken):
		self.pushToken = pushToken
		if userName:
			#create role
			self.charType = charType
			self.userName = userName
			self._checkUserName()
			return
		else:
			#now charType is filled with gid by client
			gid = charType
			if self.resultSet:
				for res in self.resultSet:
					if int(res[2]) == gid:
						ban_time = int(res[3])
						ban_reason = res[4]
						if ban_time and ban_time > time.time():
							self.safeClient.onBeenBanned(ban_time, ban_reason)
							return
						self.currentGID = gid
						if switches.DBG_GM_LOCK:
							BWEngine.getGlobalBase("RoleStub").acquireLock(self, "Avatar", gid, const.LockReason.LOGIN, "onAcquireLock")
						else:
							if self.recoverForOccChange:
								utils.loadBackupAvatarForOccupationChangeFailure(gid,
										lambda d: self.createNextFromKVMemory("Avatar", d))
								self.recoverForOccChange = False
							else:
								self.createNext("Avatar", {}, fromDB=True, playerName=str(gid))
						return
				#gids = [int(res[2]) for res in self.resultSet]
				#if gid in gids:

			self.logger.error("base\Account selectCharacter gid:%s not found"%gid)
			self.entireDestroy(False, True)
			#self.createNext("Avatar", {}, fromDB=True)

	@bwdecorator.keyrpc
	def onSelectCharacter(self, res, gid = 0):
		self.safeClient.onSelectCharacter(res, gid)

	def _selectCharacter(self, gid):
		safesuper(Account, self)._selectCharacter()
		self.onSelectCharacter(const.UserNameCheckCode.RET_OK, gid)

		self.currentGID = gid

		occ = reader.get_table("entity_table")[self.charType]["OccupType"]
		lv = reader.get_table("default_property_table")[occ].get("level")
		duty_type = reader.get_table("entity_table")[self.charType]["duty_type"]
		CampType = reader.get_table('camp_table')

		CreateRoleLog().log ({
			CreateRoleLog.AccountIdKey: self.playerName,
			CreateRoleLog.RoleIdKey: gid,
			CreateRoleLog.RoleNameKey: self.userName,
			CreateRoleLog.CreateTimeKey: int(Time.time),
			CreateRoleLog.RootMarkKey: self.clientInfo["rootMark"],
			CreateRoleLog.UrsKey: self.channelInfo['sauth'].get('origin_account', ''),
			})
		
		self.createNext("Avatar",
				{
					"playerName": self.playerName,
					"playerUUID": self.playerUUID,

					"accountName": self.playerName,
					"accountUUID": self.playerUUID,

					"roleName": self.userName,
					"roleUUID": Netease.getUUID(),
					"roleGID": gid,
					"stringGID": str(gid),
					"charType": self.charType,
					"campType": CampType.CAMP_PLAYER,
					"level" : lv,
					"duty_type": duty_type,
					"userName": self.userName,
					"gmGroup": GMAdmin.getGroup(self.playerName),
					"fromDB" : False,
					#"birthInDB" : self.birthInDB,
				})
		if switches.ENABLE_SAVE_ACCOUNT_DATA and not self.loginProxy.isBot:
			self.accountSaveRequest(gid, occ, self.playerName, BWConfigs.getServerName())

	def _selectCharacterCallback(self, resultSet, affectRows, error):
		if not self.hasClient:
			self.logger.error("[lyp] Account._selectCharacterCallback has no client. error=%s", error)
			#掉线重连开启
			self.onDisconnect(const.CLIENT_DEATH_REASON.INACTIVITY)
			return

		if resultSet is None:
			self.logger.error("[lyp] Account._selectCharacterCallback result set is none. error=%s", error)
			self.entireDestroy(False, True)
			return
		if len(resultSet) == 0:
			#名字合法, 创建avatar
			BWEngine.getGlobalBase("AdminStub").requestGid(self)
		else:
			#通知客户端  名字重复
			self.onSelectCharacter(const.UserNameCheckCode.NOT_ONLY)

	def _onSendPushRequest(self, path, param, rc, data):
		self.logger.info('[jt] _onSendPushRequest path:%s param:%s rc:%d data:%s', path, param, rc, data)
	
	def _onReservedWriteToDB(self, succ, ent):
		if succ:
			rlg = properties.getGlobalConfigData(0).ReservationLuckyGift
			self.safeClient.onReservedSucc(self._reservationPromote(), ent.occupation_type, ent.roleName, ent.roleType, ent.roleGID)
			mail  = reader.get_table('mail_content_table').get(rlg.reservationMail)
			content = '%s|%s|%s'%(mail['Type'], mail['Title'], mail['Text'])
			BWEngine.getGlobalBase("RoleStub").gmSysInformSingle(self.currentGID, content, [(rlg.reservationLuckItem, 1, {'non_persistent': {"reason" : "reservation"}})], '시스템', -1)
			BWEngine.getGlobalBase("QueueStub").onReserved(self.currentGID, self.userName)
			ent.registPushService(self.pushToken, 'reservedTag', self._onSendPushRequest)
			ent.entireDestroy(False, True)
			self.reserved = 1
			self.disconnectByServer(delay=3)
		else:
			self.safeClient.onReserved(0, '예약 실패, 잠시 후 다시 시도해주세요.')
			self.disconnectByServer(delay=3)

	def onNextCreated(self, fromDB):
		if not self.reservation:
			safesuper(Account, self).onNextCreated(fromDB)

		self.nextProxy.fromDB = fromDB
		self.nextProxy.aid = self.aid
		if not fromDB:
			self.nextProxy.initDefaultPackageItem()
		self.nextProxy.setClientConfig(self.clientConfig)
		if not fromDB and switches.DBG_FRESH_AUTO_WRITETODB:
			self.writeToDB()
			self.nextProxy.initForFirstCreate()
			if self.reservation:
				self.nextProxy.writeToDB(self._onReservedWriteToDB)
			else:
				self.nextProxy.writeToDB(self.nextProxy.onFirstWriteToDB )

	def _hasAvatar(self):
		sql = bsql.SQL_HAS_AVATAR % Netease.mysqlEscape(self.playerName)
		BigWorld.executeRawDatabaseCommand(sql, self._hasAvatarCallback)

	def _hasNoAvatarCallback(self):
		self.askSelectCharacter([])
		safesuper(Account, self)._hasNoAvatarCallback()

	def _hasAvatarCallback(self, resultSet, affectRows, error):
		self.resultSet = resultSet
		if not self.hasClient:
			self.logger.error("[lb] Account._hasAvatarCallback has no client. error=%s", error)
			#掉线重连开启
			self.onDisconnect(const.CLIENT_DEATH_REASON.INACTIVITY)
			return
		if resultSet is None:
			self.logger.error("[lb] Account._hasAvatarCallback result set is none. error=%s", error)
			self.entireDestroy(False, True)
			return

		if len(resultSet) == 0:
			# has no avatar in db
			self._hasNoAvatarCallback()
		else:
			#ban_time = int(resultSet[0][3])
			#ban_reason = resultSet[0][4]
			#if ban_time and ban_time > time.time():
			#	self.safeClient.onBeenBanned(ban_time, ban_reason)
			#	return
			self._askSelectCharacter()

	def onAcquireLock(self, succ, entType, gid, lockReason):
		#返回是登录锁 代表没有上其他锁 可以登录
		if lockReason == const.LockReason.LOGIN:
			if self.recoverForOccChange:
				utils.loadBackupAvatarForOccupationChangeFailure(gid,
						lambda d: self.createNextFromKVMemory("Avatar", d))
				self.recoverForOccChange = False
			else:
				self.createNext("Avatar", {}, fromDB=True, playerName=str(gid))
		else:
			self.logger.info("[lyplog] acquire lock fail, succ:%s entType:%s gid:%s lockReason:%s"%(succ, entType, gid, lockReason))

	def onRecoveringGIDTimer(self, timerID=None, userData=None):
		self.selectCharacter(userData, '', self.pushToken)

#	def entireDestroy(self, deleteFromDB, writeToDB):
#		import traceback
#		traceback.print_stack()
#		super(Account, self).entireDestroy(deleteFromDB, writeToDB)
#
#	def onNextDestroyed(self):
#		self.nextProxy = None
#		if not self.reservation and self.hasClient:
#			pass
#		else:
#			self.destroyEntity()
#
	def hasAvatar(self, gid):
		if not self.resultSet:
			return False
		for res in self.resultSet:
			if int(res[2]) == gid:
				return True
		return False


	def _askSelectCharacter(self):
		if self.reservation:
			if self.resultSet:
				self.safeClient.onReserved(1, self._reservationPromote())
				self.disconnectByServer(delay=3)
				return
		if self.avatarInfo is None:
			self.avatarInfo = {}

		done = False
		if self.recoveringGID:
			self.recoveringGID, gid = 0, self.recoveringGID
			for res in self.resultSet:
				if int(res[2]) == gid:
					done = True
					self.logger.info('_askSelectCharacter resultSet=%s, recoveringGID:%s', self.resultSet, gid)

					self.registTimer(1, "onRecoveringGIDTimer", 0, exclude=True, userData=gid)
					break

		if not done:
			accounts = []
			for res in self.resultSet:
				gid = int(res[2])
				if self.avatarInfo.get(gid):
					onEquipIds = self.avatarInfo[gid]["onEquipId"]
					onTransmogrification = self.avatarInfo[gid].get('onTransmogrification', [0] * 30)
				else:
					onEquipIds = [0] * 30
					onTransmogrification = [0] * 30

				accounts.append({
					"gid" : gid,
					"charType" : int(res[5]),
					"level" : int(res[6]),
					"name" : res[7],
					"onEquipId" : onEquipIds,
					"onTransmogrification" : onTransmogrification,
					})
			self.askSelectCharacter(accounts)
	
	def askSelectCharacter(self, accounts):
		if self.loginProxy.hasNames:
			self.safeClient.askSelectCharacter(accounts, self.loginProxy.hasNames)
		else:
			#没拉到 等1秒
			self.registTimer(1, "onTimerAskSelectCharacter", userData = accounts, exclude=True)

	def onTimerAskSelectCharacter(self, timerid, userdata):
		self.safeClient.askSelectCharacter(userdata, self.loginProxy.hasNames)

	def decodeSession(self, password):
		# default passwd
		# AS = 'bfd16447'
		# BS = 'dbedbe98'
		AS_encoded = 'aec05336'
		# default AS +BS encoded aec05336cadcad87
		if password.startswith(AS_encoded):
			return True, ''.join([chr(ord(ch)+1)for ch in password[len(AS_encoded):]])
		else:
			return False, ''

	def onDisconnect(self, reason):
		super(Account, self).onDisconnect(reason)
		if self.nextProxy:
			return

		if getattr(self, "inQueue", False):
			if self.destroyTimerID:
				self.cancelTimer(self.destroyTimerID)
				self.destroyTimerID = 0
			self.destroyTimerID = self.registTimer(const.ACCOUNT_DISCONNECT_TIME_IN_QUEUE, "onTimerDestroy", exclude=True)
			return

		self.destroyEntity()

	def onTimerDestroy(self, timerID, userData):
		self.destroyTimerID = 0
		if not self.nextProxy:
			self.destroyEntity()

	def onBillboard(self, content):
		"""需要产生公告回调, 向客户端推送登录公告"""
		#content = reader.get_table("announcement_table")[1]["announce_content"]
		self.safeClient.onBillboard(content)

	def onNotice(self, noticeType, content):
		"""推送类公告"""
		#content = reader.get_table("announcement_table")[2]["announce_content"]
		self.safeClient.onNotice(noticeType, content)

	@bwdecorator.keyrpc
	def onRequestGid(self, gid):
		self._selectCharacter(gid)

	@bwdecorator.keyrpc
	def criticalRecover(self, gid, is_occrecover):
		self.recoveringGID = gid
		self.recoverForOccChange = is_occrecover

	@bwdecorator.keyrpc
	def onDuplicateLogin(self):
		proxy = self
		while proxy.nextProxy:
			proxy = proxy.nextProxy
		proxy.entireDestroy(False, True)

	def updateChannelInfo(self, channelInfo, clientInfo):
		self.channelInfo = channelInfo
		self.clientInfo = clientInfo
		if self.nextProxy:
			self.nextProxy.updateChannelInfo(channelInfo, clientInfo)

	def updateLoginProxy(self, loginProxy):
		self.loginProxy = loginProxy

	def _preEntireDestroy(self):
		safesuper(Account, self)._preEntireDestroy()
		self.setOffline()
		hasattr(self, "loginProxy") and self.loginProxy.onAccountDestroy(self.id)

	def updateEquipInfo(self, gid, onEquipId):
		if not self.avatarInfo.has_key(gid):
			self.avatarInfo[gid] = {}
		self.avatarInfo[gid]["onEquipId"] = list(onEquipId)

	def updateOnTransmogrification(self, gid, onTransmogrification):
		if not self.avatarInfo.has_key(gid):
			self.avatarInfop[gid] = {}
		self.avatarInfo[gid]['onTransmogrification'] = list(onTransmogrification)

	def delAvatarRequest(self, gid):
		if self.resultSet:
			gids = [int(res[2]) for res in self.resultSet]
			if gid in gids:
				if self.currentGID == gid:
					self.logger.error("base\Account delAccountRequest gid:%s avatar online"%gid)
					return
				sql = bsql.SQL_DEL_AVATAR % Netease.mysqlEscape(gid)
				BigWorld.executeRawDatabaseCommand(sql, self._delAvatarCallback)
				self.avatarInfo.pop(gid)
				return
		self.logger.error("base\Account delAccountRequest gid:%s not found"%gid)

	def _delAvatarCallback(self, resultSet, affectRows, error):
		print '[lyplog] base/Account _delAvatarCallback', resultSet, affectRows, error
		#TODO
	
	@bwdecorator.keyrpc
	def createAvatarFromSample(self, sampleName):
		def fetchGid():
			from admin import AdminClient
			import BWConfigs
			from admin.proto_python import admin_pb2
			admin_client = AdminClient.AdminClient()
			admin_client._admin_stub.RequestGid(None, admin_pb2.Gid(host_id=BWConfigs.getHostID(), count=1), onFetchGid)

		def onFetchGid(gid):
			if gid is None:
				self.fetching_gid = False
				self._fetchGid()
				return
			self.copyAvatarGid = gid.gid
			BWEngine.getGlobalBase("RoleStub").callAvatarFuncByUrsWithFailCallback(sampleName, {"func" : "copyAvatar", "args" : (sampleName, self)}, True, self, {"func" : "onCopyAvatarFail", "args" : (sampleName, )})
		fetchGid()

	def onCopyAvatar(self, sampleName, data):
		import base64
		data = base64.b64decode(data)
		self._createCopyAvatar(data)

	def _createCopyAvatar(self, data):
		self.userName = self.playerName
		self.roleUUID = Netease.getUUID()
		self.account_id = self.playerName
		params = {
			"playerName": self.playerName,
			"playerUUID": self.roleUUID,

			"accountName": self.account_id,
			"accountUUID": self.roleUUID,

			"roleName": self.userName,
			"roleUUID": Netease.getUUID(),
			"roleGID": self.copyAvatarGid,
			"stringGID": str(self.copyAvatarGid),
			"userName": self.userName,
			"gmGroup": GMAdmin.getGroup(self.playerName),
		}
		avatar = BigWorld.createBaseFromKVMemory("Avatar", data, params)
		self.giveClientTo(avatar)
		avatar.prevProxy = self
		self.nextProxy = avatar

	def changeRoleOccupation(self, tar_chartype):
		occ = reader.get_table("entity_table")[tar_chartype]["OccupType"]
		self.accountSaveRequest(self.currentGID, occ, self.playerName, BWConfigs.getServerName())
	
